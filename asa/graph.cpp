#include "graph.hpp"
#include "fasta.hpp"
#include "bfs_state.hpp"
#include "traversal_state.hpp"

#include <algorithm>
#include <iterator>
#include <cassert>
#include <numeric>
#include <cmath>

/*  1 << 20   -   1 Megabyte
 *  1 << 21   -   2 Megabyte
 *  1 << 24   -  16 Megabyte
 */
constexpr static const std::size_t M = 1 << 24; // <- size of memory partition when constructing cFP
constexpr static const Nucleotide nucleotides[] = { Nucleotide::A, Nucleotide::C, Nucleotide::G, Nucleotide::T };

graph::graph(std::istream& reads, std::uint_fast64_t kmer_count, std::size_t kmer_size)
  : reads(reads), kmer_size(kmer_size)
{
  const double F = 1. / (bloom_params::Fc * kmer_size);
  const std::size_t n = kmer_count;
  const double m = 1.44 * n * std::log2(1. / F);
                        // allocate at most 1GB, just an eyeballed, probably poor decision
  const std::size_t tm = std::min(std::size_t(1 << 30), static_cast<std::size_t>(m + 0.5));
  filter = std::make_unique<bloom_filter>(tm, 0.7 * (tm / n));
                                           // optimal parameters as described by Chikhi and Rizk in 2013
  read_fasta(reads) |
    [this, kmer_size](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;

      for(std::size_t i = 0; i <= read.size() - kmer_size; ++i)
      {
        auto tmp = read.substr(i, kmer_size);

        auto nc = Nucleotides::from_string(tmp);
        auto tw = nc.twin();

        filter->add(nc < tw ? nc : tw);
      }
    };

  // compute cFP
  cFP.reserve(tm);
  {
  read_fasta(reads) |
    [this,kmer_size](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;
      for(std::size_t k = 0; k <= read.size() - kmer_size; ++k)
      {
        auto tmp = Nucleotides::from_string(read.substr(k, kmer_size));
        std::vector<Nucleotides> unchecked_ext;
        {
        auto pred_kmer = tmp.front_tail(); // <- AGT becomes AG
        for(auto c : nucleotides)
        {
          auto ncs = c + pred_kmer;
          auto tw = ncs.twin();

          if(ncs < tw && filter->contains(ncs))
            unchecked_ext.push_back(ncs);
          else if(filter->contains(tw))
            unchecked_ext.push_back(tw);
        }
        }
        {
        auto succ_kmer = tmp.tail(); // <- AGT becomes GT
        for(auto c : nucleotides)
        {
          auto ncs = succ_kmer + c;
          auto tw = ncs.twin();

          if(ncs < tw && filter->contains(ncs))
            unchecked_ext.push_back(ncs);
          else if(filter->contains(tw))
            unchecked_ext.push_back(tw);
        }
        }
        for(auto e : unchecked_ext)
          cFP.insert(e);
      }
    };
  read_fasta(reads) |
    [this,kmer_count,kmer_size, j = 0U, i = 0U, part = std::set<Nucleotides>()]
    (const std::string& read) mutable
    {
      if(j < M && i < kmer_count)
      {
        if(read.size() < kmer_size)
          return;
        for(std::size_t k = 0; k <= read.size() - kmer_size; ++k)
        {
          auto tmp = read.substr(k, kmer_size);
          Nucleotides ncs = Nucleotides::from_string(tmp);
          Nucleotides tw = ncs.twin();

          part.insert(ncs < tw ? ncs : tw);
        }
        i += read.size() - kmer_size + 1;
        j += read.size() - kmer_size + 1;
      }
      if(j >= M || i >= kmer_count)
      {
        // now, we either have read all kmers or are done with our first partition of size M
        j = 0;
        assert(i <= kmer_count);

        for(auto it = cFP.begin(); it != cFP.end(); )
        {
          auto& kmer = *it;

          // part contains valid stuff, so if a kmer from our current critical false positive
          // set is in it, we should remove it from the cFP set
          if(part.find(kmer) != part.end())
            it = cFP.erase(it);
          else
            ++it;
        }
      }
    };
  }
  // find branches
  {
  read_fasta(reads) |
    [this,kmer_size](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;

      for(std::size_t i = 0; i <= read.size() - kmer_size; ++i)
      {
        auto kmer = Nucleotides::from_string(read.substr(i, kmer_size));
        auto ckmer = kmer.twin();

        if(kmer < ckmer && is_branch(Node{Dir::Forward, kmer}))
        {
          Node to_insert { Dir::Forward, kmer };

          auto lb = std::lower_bound(branches.begin(), branches.end(), to_insert);
          if(lb == branches.end() || to_insert < *lb)
            branches.insert(lb, to_insert);
        }
        else if(is_branch(Node{Dir::Backward, ckmer}))
        {
          Node to_insert { Dir::Backward, ckmer };

          auto lb = std::lower_bound(branches.begin(), branches.end(), to_insert);
          if(lb == branches.end() || to_insert < *lb)
            branches.insert(lb, to_insert);
        }
      }
    };
  }
}

std::size_t graph::indegree(const Node& kmer) const
{ return predecessors(kmer).size(); }

std::size_t graph::outdegree(const Node& kmer) const
{ return successors(kmer).size(); }

std::vector<Node> graph::extensions(const Node& other) const
{
  // basically yield "successors" or "predecessors" depending on a node's orientation
  if(other.direction == Dir::Forward)
  {
    auto kmer = other.ncs.tail(); // <- AGT becomes GT

    std::vector<Node> to_return;
    to_return.reserve(4);
    for(auto c : nucleotides)
      if(likely_contains(kmer + c))
        to_return.push_back({ Dir::Forward, kmer + c });
    return to_return;
  }
  else
  {
    auto kmer = other.ncs.front_tail(); // <- AGT becomes AG

    std::vector<Node> to_return;
    to_return.reserve(4);
    for(auto c : nucleotides)
      if(likely_contains(c + kmer))
        to_return.push_back({ Dir::Backward, c + kmer });
    return to_return;
  }
}

std::vector<Edge> graph::extensions(const Node& other, Dir dir) const
{
  // yield all neighbours of a node in a certain direction 
  Nucleotides ncs = (other.direction == Dir::Forward ? other.ncs : other.ncs.twin());

  if(dir == Dir::Forward)
  {
    auto kmer = ncs.tail(); // <- AGT becomes GT

    std::vector<Edge> to_return;
    to_return.reserve(4);
    for(auto c : nucleotides)
    {
      auto fw = kmer + c;
      auto bw = fw.twin();

      if(fw < bw && likely_contains(fw, bw))
        to_return.push_back({ other, Node{ Dir::Forward, fw }, c, Dir::Forward });
      else if(bw < fw && likely_contains(bw, fw))
        to_return.push_back({ other, Node{ Dir::Backward, bw }, c, Dir::Forward });
    }
    return to_return;
  }
  else
  {
    auto kmer = ncs.front_tail(); // <- AGT becomes AG

    std::vector<Edge> to_return;
    to_return.reserve(4);
    for(auto c : nucleotides)
    {
      auto fw = c + kmer;
      auto bw = fw.twin();

      if(fw < bw && likely_contains(fw, bw))
        to_return.push_back({ other, Node{ Dir::Forward, fw }, c, Dir::Backward });
      else if(bw < fw && likely_contains(bw, fw))
        to_return.push_back({ other, Node{ Dir::Backward, bw }, c, Dir::Backward });
    }
    return to_return;
  }
}

Node graph::extension(const Node& other, Dir dir, Nucleotide nt) const
{
  // Extend a node in a certain direction, i.e. AGT to GTT
  // We need this method, since k-mers are identified with their reverse complements in our graph
  Nucleotides ncs = (other.direction == Dir::Forward ? other.ncs : other.ncs.twin());

  if(dir == Dir::Forward)
  {
    auto fw = ncs.tail() + nt;
    auto bw = fw.twin();

    if(fw < bw && likely_contains(fw, bw))
      return Node{ Dir::Forward, fw };
    else if(bw < fw && likely_contains(bw, fw))
      return Node{ Dir::Backward, bw };
  }
  else
  {
    auto fw = nt + ncs.front_tail();
    auto bw = fw.twin();

    if(fw < bw && likely_contains(fw, bw))
      return Node{ Dir::Forward, fw };
    else if(bw < fw && likely_contains(bw, fw))
      return Node{ Dir::Backward, bw };
  }
  assert(false && "Requested non-existent node.");
  std::exit(-1); // <- really exit here, even in release mode. The results would be undefined if we continue!
  return Node();
}

bool graph::likely_contains(const Nucleotides& kmer) const
{
  auto ckmer = kmer.twin();

  if(kmer < ckmer)
    return filter->contains(kmer) && cFP.find(kmer) == cFP.end();
  return filter->contains(ckmer) && cFP.find(ckmer) == cFP.end();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool graph::likely_contains(const Nucleotides& kmer, const Nucleotides& twin) const
{
  // assumes that kmer < twin and that twin is the actual twin of the kmer
  assert(kmer < twin); 
  assert(kmer.twin() == twin);

  return filter->contains(kmer) && cFP.find(kmer) == cFP.end();
}
#pragma GCC diagnostic pop

std::vector<Node> graph::predecessors(const Node& n) const
{ return extensions({ flip(n.direction), n.ncs }); }

std::vector<Node> graph::successors(const Node& n) const
{ return extensions(n); }

std::optional<Edge> graph::simple_traversal(const Node& start, Dir dir) const
{
  auto succs = extensions(start, dir);
  auto preds = extensions(start, flip(dir));
  if(succs.size() == 1) // start must not have branches to multiple targets
  {
    // target must not have multiple incoming edges
    if(extensions(start, flip(dir)).size() > 1)
      return std::nullopt;
    return succs.front();
  }
  return std::nullopt;
}

bool graph::is_branch(const Node& kmer) const
{ return indegree(kmer) != 1 || outdegree(kmer) != 1; }

bool graph::complex_traversal(const Node& start, Dir dir, traversal_state& trav, Nucleotides& out, const Node& prev) const
{
  static constexpr std::size_t maximum_depth = 250;
  static constexpr std::size_t maximum_fatness = 50;

  std::set<Node> new_marked;
  std::size_t deepness = 0;
  Node end;
  {
  bool stop = false;
  bfs_state state(start, prev, dir, *this, new_marked, trav);
  do {
    if(++state)
      return true;

    if(state.deepness() > maximum_depth || state.fatness() > maximum_fatness || state.fatness() == 0)
      return true;
    if(state.fatness() == 1 && !is_branch(state.front()))
    { end = state.front(); deepness = state.deepness(); stop = true; }
  } while(!stop);
  }
  // enumerate all paths between start and end in a recursive dfs-manner
  // should reflect what we previously saw in our bfs traversal
  auto all_paths = [this,dir,&end](const Node& start, Nucleotides state, std::set<Nucleotides> marked, int depth, auto recursor)
             {
               std::set<Nucleotides> buffer;
               if(depth < -1)
                 return std::make_pair(true, buffer);
               if(start.ncs == end.ncs)
               {
                 buffer.insert(state);
                 return std::make_pair(false, buffer);
               }
               auto exts = extensions(start, dir);
               for(auto e : exts)
               {
                 if(marked.find(e.to.ncs) != marked.end())
                   return std::make_pair(true, buffer);

                 Nucleotides new_state = state;
                 new_state += e.nt;
                 
                 std::set<Nucleotides> new_marked = marked;
                 new_marked.insert(e.to.ncs);

                 auto [failure, result] = recursor(e.to, new_state, new_marked, depth - 1, recursor);
                 buffer.insert(result.begin(), result.end());

                 if(buffer.size() > maximum_fatness)
                   return std::make_pair(true, buffer);
               }
               return std::make_pair(false, buffer);
             };
  std::set<Nucleotides> marked;
  marked.insert(start.ncs);

  Nucleotides state;
  if(auto [quit, buff] = all_paths(start, state, marked, deepness + 1, all_paths); quit)
    return true;
  else
    out = *buff.begin();
  for(auto& a : new_marked)
    trav.mark(a);

  return false;
}

const std::vector<Node>& graph::all_branches() const
{ return branches; }

void graph::print(std::ostream& os)
{
  os << "digraph deBrujin {\n";

  std::set<std::string> to_print;
  read_fasta(reads) |
    [this,&os,&to_print](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;

      for(std::size_t i = 0; i <= read.size() - kmer_size; ++i)
      {
        auto kmer_s = read.substr(i, kmer_size);

        auto kmer = Nucleotides::from_string(kmer_s);
        auto s = successors(Node{ Dir::Forward, kmer });
        for(auto succ : s)
          to_print.insert(kmer_s + " -> " + succ.ncs.to_string() + ";\n");

        auto ckmer = kmer.twin();
        auto ckmer_s = ckmer.to_string();
        s = successors(Node{ Dir::Backward, ckmer });
        for(auto succ : s)
          to_print.insert(ckmer_s + " -> " + succ.ncs.to_string() + ";\n");
      }
    };
  for(auto& s : to_print)
    os << s;
  /*
  for(auto& s : cFP)
  {
    auto preds = predecessors(s);
    for(auto pred : preds)
      os << pred.to_string() + " -> " + s.to_string() + ";\n";
    os << s.to_string() << " [style=dotted];\n";
  }
  */
  os << "}\n";

}

