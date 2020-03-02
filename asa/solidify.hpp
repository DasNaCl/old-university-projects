#pragma once

#include <cstdint>
#include <iosfwd>
#include <limits>

// any kmer with occurence number above this is considered solid
constexpr static const std::size_t min_occurence = 2;
constexpr static const std::size_t max_occurence = std::numeric_limits<unsigned short>::max() - 1;

// reads fasta file from istream and computes an approximate k-mer count
// with that we can distinct between weak and solid kmers
// a kmer is solid iff it occurs often enough
// writes solid kmers to intermediate buffer os
// returns the number of solid kmers
std::uint_fast64_t solidify(std::size_t kmer_size, std::uint_fast64_t kmer_count, std::istream& is, const std::string& outname);

