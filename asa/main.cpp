#include "graph.hpp"
#include "subtask_a.hpp"
#include "contig_writer.hpp"
#include "traversal_state.hpp"
#include "solidify.hpp"
#include "fasta.hpp"

#include <filesystem>
#include <iostream>
#include <cassert>
#include <fstream>
#include <future>
#include <string>

int main(int argc, char** argv)
{
  if(argc != 5)
  {
    std::cout << "Argument count mismatch. Expected was: 4\n";
    return 1;
  }
  std::fstream input(argv[1], std::ios::in);
  std::size_t size = std::stoull(argv[3]);
  std::fstream output(argv[2], std::ios::out);

  { std::ofstream(".assemble_0", std::ios::out); }
  std::uint_fast64_t kmer_count = 0;
  {
  contig_writer cw(output, 80, size);
  read_fasta(input) |
    [size,&kmer_count,&cw](const std::string& read)
    {
      if(read.size() < size)
        return;
      kmer_count += read.size() - size + 1;
      cw << read;
    };
  }
  std::thread t([argv,kmer_count,size]()
      {
        { std::filesystem::remove(std::filesystem::path(".assemble_0")); std::ofstream(".assemble_1", std::ios::out); }
        std::ifstream input(argv[1]);
        graph g(input, kmer_count, size);
      //        g.print(std::cerr);
        subtask_a(g, Nucleotides::from_string(argv[4]));
        std::filesystem::remove(std::filesystem::path(".assemble_1"));
      });
  output.close();
  input.close();

  const std::string corrected_file = argv[2] + std::string("_corr");
  std::string to_open = argv[2];

  input.open(to_open, std::ios::in);
  // now we are allowed to error correct
  std::uint_fast64_t solid_kmer_count = solidify(size, kmer_count, input, corrected_file);
  input.close();
  input.open(corrected_file, std::ios::in);

  { std::ofstream(".assemble_3", std::ios::out); }
  graph g(input, solid_kmer_count, size);
  input.close();

  auto branches = g.all_branches();

  output.open(argv[2], std::ios::out | std::ios::trunc);
  contig_writer cw(output, 80, size);

  { std::filesystem::remove(std::filesystem::path(".assemble_3")); std::ofstream(".assemble_4", std::ios::out); }
  traverse(g, branches, cw);

  std::filesystem::remove(std::filesystem::path(".assemble_4"));
  std::filesystem::remove(std::filesystem::path(corrected_file));

  t.join();

  return 0;
}
