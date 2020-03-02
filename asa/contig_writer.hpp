#pragma once

#include <iosfwd>
#include <string>
#include <vector>

struct Nucleotides;

struct contig_writer
{
  contig_writer(std::ostream& os, std::size_t w, std::size_t kmer_size)
    : os(os), w(w), contig_count(0), k(kmer_size)
  {  }

  friend contig_writer& operator<<(contig_writer& cw, const Nucleotides& contig);
  friend contig_writer& operator<<(contig_writer& cw, const std::string& contig);

  std::uint_fast64_t write_count() const
  { return contig_count; }
private:
  std::ostream& os;
  std::size_t w;

  std::uint_fast64_t contig_count;

  std::size_t k;
};

