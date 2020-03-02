#pragma once

#include "nucleotides.hpp"

#include <string>
#include <queue>
#include <set>

struct graph;
struct traversal_state;

struct bfs_state
{
  bfs_state(const Node& start, const Node& prev, Dir dir, const graph& g, std::set<Node>& marked, traversal_state& trav);

  operator bool() const;
  bfs_state& operator++();

  std::size_t deepness() const;
  std::size_t fatness() const;
  const Node& front() const;
private:
  std::queue<Node> visiting_nodes;
  const graph& g;
  std::size_t depth;

  std::set<Node> seen;
  std::set<Node>& marked;

  bool dirty_flag;
  traversal_state& trav;

  Dir dir;
};

