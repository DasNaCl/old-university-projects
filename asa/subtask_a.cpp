#include "subtask_a.hpp"
#include "nucleotides.hpp"
#include "graph.hpp"

#include <iostream>

void subtask_a(graph& g, const Nucleotides& query_kmer)
{
  auto preds = g.predecessors({ Dir::Forward, query_kmer });
  auto succs = g.successors({ Dir::Forward, query_kmer });

  // query is probably not in the graph, hence it has no predecessors or successors
  if(!g.likely_contains(query_kmer) || query_kmer.ntcount() != g.k())
  {
    preds.clear();
    succs.clear();
  }

  std::cout << "Incoming k-mers - " << (preds.empty() ? "None" : "");
  for(std::size_t i = 0; i < preds.size(); ++i)
  {
    auto c = preds[i];
    std::cout << c.ncs.to_string();
    if(i + 1 < preds.size())
      std::cout << ",";
  }
  std::cout << "\n";

  std::cout << "Outgoing k-mers - " << (succs.empty() ? "None" : "");
  for(std::size_t i = 0; i < succs.size(); ++i)
  {
    auto c = succs[i];
    std::cout << c.ncs.to_string();
    if(i + 1 < succs.size())
      std::cout << ",";
  }
  std::cout << "\n";
}

