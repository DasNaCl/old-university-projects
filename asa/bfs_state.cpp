#include "bfs_state.hpp"
#include "traversal_state.hpp"
#include "graph.hpp"

bfs_state::bfs_state(const Node& start, const Node& prev, Dir dir, const graph& g, std::set<Node>& marked, traversal_state& trav)
  : g(g), depth(0), marked(marked), dirty_flag(false), trav(trav), dir(dir)
{
  seen.insert(start);
  seen.insert(prev);
  visiting_nodes.emplace(start);
}

bfs_state::operator bool() const
{ return dirty_flag; }

bfs_state& bfs_state::operator++()
{
  std::queue<Node> fifo;
  while(!visiting_nodes.empty())
  {
    auto current = visiting_nodes.front();
    visiting_nodes.pop();

    auto succs = g.extensions(current, dir);
    for(auto edge : succs)
    {
      auto neighbor = edge.to;
      if(seen.find(neighbor) != seen.end())
        continue;

      if(g.is_branch(neighbor) && trav.is_marked_branch(neighbor))
      {
        dirty_flag = true;
        break;
      }
      
      fifo.push(neighbor);
      seen.insert(neighbor);
      marked.insert(neighbor);
    }
  }
  visiting_nodes = fifo;
  ++depth;

  return *this;
}

std::size_t bfs_state::deepness() const
{ return depth; }

std::size_t bfs_state::fatness() const
{ return visiting_nodes.size(); }

const Node& bfs_state::front() const
{ return visiting_nodes.front(); }

