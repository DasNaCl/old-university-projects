#pragma once

#include "bloom_filter.hpp"
#include "nucleotides.hpp"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iosfwd>
#include <set>

struct traversal_state;

// based on Chikhi and Rizk, 2013
struct graph
{
  graph(std::istream& reads, std::uint_fast64_t, std::size_t kmer_size);

  std::size_t indegree(const Node& kmer) const;
  std::size_t outdegree(const Node& kmer) const;

  std::vector<Node> extensions(const Node& other) const;
  std::vector<Edge> extensions(const Node& other, Dir dir) const;

  Node extension(const Node& other, Dir dir, Nucleotide nt) const;

  std::vector<Node> predecessors(const Node& n) const;
  std::vector<Node> successors(const Node& n) const;

  bool likely_contains(const Nucleotides& kmer) const;
  bool likely_contains(const Nucleotides& kmer, const Nucleotides& twin) const;

  std::optional<Edge> simple_traversal(const Node& start, Dir dir) const;

  bool complex_traversal(const Node& start, Dir dir, traversal_state& trav, Nucleotides& path, const Node& prev) const;

  const std::vector<Node>& all_branches() const;

  const std::unordered_set<Nucleotides, NucleotidesHasher>& critical_false_positives() const
  { return cFP; }

  void print(std::ostream& out);

  bool is_branch(const Node& kmer) const;

  std::size_t k() const
  { return kmer_size; }
private:
  std::unique_ptr<bloom_filter> filter;
  std::unordered_set<Nucleotides, NucleotidesHasher> cFP;

  std::vector<Node> branches;

  std::istream& reads;
  std::size_t kmer_size;
};

std::string reverse_complement(const std::string& kmer);

