#include "traversal_state.hpp"
#include "contig_writer.hpp"
#include "bfs_state.hpp"
#include "graph.hpp"

#include <cassert>

traversal_state::traversal_state(const graph& g)
  : g(g)
{
  for(auto b : g.all_branches())
    branching_mers[b.ncs] = 0;
}

int offset(const Edge& e)
{ return (e.dir == e.from.direction ? 0 : 4); }

Edge flip(const Edge& e)
{
  Nucleotide nt = e.nt;
  if(e.dir == Dir::Forward)
  {
    if(e.from.direction == Dir::Forward)
      nt = *(e.from.ncs.end() - 1);
    else
    {
      const auto& f = e.from.ncs.twin();
      nt = *(f.end() - 1);
    }
  }
  else
  {
    if(e.from.direction == Dir::Forward)
    {
      const auto& f = e.from.ncs.twin();
      nt = *(f.end() - 1);
    }
    else
      nt = *(e.from.ncs.end() - 1);
  }
  return { e.to, e.from, nt, flip(e.dir) };
}

std::vector<Edge> all_extensions(const graph& g, const Node& node)
{
  std::vector<Edge> total;

  auto succs = g.extensions(node, Dir::Forward);
  auto preds = g.extensions({ flip(node.direction), node.ncs }, Dir::Forward);

  total.insert(total.end(), succs.begin(), succs.end());
  total.insert(total.end(), preds.begin(), preds.end());

  return total;
}

void traversal_state::mark(const Node& node)
{
  if(auto it = branching_mers.find(node.ncs); it != branching_mers.end())
    it->second |= (1 << 8);

  for(auto& e : all_extensions(g, node))
  {
    if(branching_mers.count(e.to.ncs) == 0)
      continue;
    mark(flip(e));
  }
}

bool traversal_state::is_marked(const Node& node) const
{
  if(branching_mers.count(node.ncs))
    return is_marked_branch(node);
  for(auto& e : all_extensions(g, node))
  {
    if(branching_mers.count(e.to.ncs) == 0)
      continue;
    if(is_marked(flip(e)))
      return true;
  }
  return false;
}

bool traversal_state::is_marked_branch(const Node& node) const
{ return (branching_mers.at(node.ncs) & (1 << 8)) != 0; }

void traversal_state::mark(const Edge& e)
{
  if(branching_mers.count(e.from.ncs) == 0)
    return;

  branching_mers[e.from.ncs] |= (1 << (e.nt + offset(e)));
}

bool traversal_state::is_marked(const Edge& e) const
{
  if(auto it = branching_mers.find(e.from.ncs); it != branching_mers.end())
    return (it->second >> (e.nt + offset(e))) & 0b1;
  return false;
}

bool traversal_state::find_start(const Node& branch, Node& buffer)
{
  for(auto edge : all_extensions(g, branch))
  {
    if(g.is_branch(edge.to) || is_marked(edge.to) || is_marked(edge))
      continue;
    mark(edge);

    auto opt = g.simple_traversal(edge.to, edge.dir);
    if(opt)
    {
      // we can start collapsing nodes with indeg = 1 and outdeg = 1 from here
      
      buffer = opt.value().to;
      buffer.direction = Dir::Forward;
      return true;
    }
  }
  return false;
}

std::size_t traversal_state::traverse_step(const Node& current, Dir dir, Nucleotides& path, const Node& prev)
{
  auto opt = g.simple_traversal(current, dir);
  if(opt)
  {
    path[0] = opt.value().nt;
    return 1;
  }

  bool succ = g.complex_traversal(current, dir, *this, path, prev);
  if(succ)
    return 0;

  return path.ntcount();
}

int traversal_state::traverse(const Node& start, Dir dir, Nucleotides& total)
{
  Node current = start;
  Node prev;

  bool loop = false;

  Nucleotides buffer(1);
  
  for(std::size_t len = traverse_step(current, dir, buffer, prev); len > 0; len = traverse_step(current, dir, buffer, prev))
  {
    for(std::size_t i = 0; i < len; ++i)
    {
      total += buffer[i];

      prev = current;
      current = g.extension(current, dir, buffer[i]);
      mark(current);

      loop = loop || current.ncs == start.ncs;
    }
    if(loop)
      break;
    buffer = Nucleotides(1);
  }
  return total.ntcount();
}

void traverse(const graph& g, const std::vector<Node>& branches, contig_writer& cw)
{
  traversal_state state(g);

  for(auto b : branches)
  {
    Node start;

    while(state.find_start(b, start))
    {
      Nucleotides right;
      Nucleotides left;

      int len_r = state.traverse(start, Dir::Forward, right);
      int len_l = state.traverse(Node{ flip(start.direction), start.ncs }, Dir::Forward, left);

      Nucleotides ncs;
      for(int i = 0; i < len_l; ++i)
      {
        Nucleotide nt = left[len_l - i - 1];
        ncs.append(static_cast<Nucleotide>((nt ^ 0xA) & 0b11));
      }
      if(start.direction == Dir::Forward)
        for(auto c : start.ncs)
          ncs.append(c);
      else
        for(auto c : start.ncs.twin())
          ncs.append(c);
      for(int i = 0; i < len_r; ++i)
        ncs.append(right[i]);

      if(ncs.ntcount() < g.k())
        continue;
      cw << ncs;
      cw << ncs.twin();
    }
  }
}

