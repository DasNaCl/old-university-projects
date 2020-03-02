#pragma once

#include "nucleotides.hpp"

#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <map>

struct graph;
struct contig_writer;

void traverse(const graph& g, const std::vector<Node>& branches, contig_writer& cw);

struct traversal_state
{
  friend void traverse(const graph& g, const std::vector<Node>& branches, contig_writer& cw);
public:
  void mark(const Node& kmer);
  bool is_marked(const Node& kmer) const;
  void mark(const Edge& kmers);
  bool is_marked(const Edge& kmer) const;
  bool is_marked_branch(const Node& node) const;
private:
  traversal_state(const graph& g);

  int traverse(const Node& start, Dir dir, Nucleotides& total);

  std::size_t traverse_step(const Node& current, Dir dir, Nucleotides& path, const Node& prev);
  bool find_start(const Node& branch, Node& buffer);
private:
  const graph& g;
  std::map<Nucleotides, unsigned short> branching_mers;
};


