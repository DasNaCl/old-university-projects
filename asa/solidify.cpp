#include "solidify.hpp"
#include "bloom_filter.hpp"
#include "contig_writer.hpp"
#include "nucleotides.hpp"
#include "fasta.hpp"

#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <unordered_map>

struct KmerIsotope
{ std::size_t pos; char ch; Nucleotide nt; short score; };

short score(Nucleotide ref, Nucleotide from)
{
  // we also consider the biological context
  if(ref == Nucleotide::A && from == Nucleotide::G)
    return 2;
  else if(ref == Nucleotide::G && from == Nucleotide::A)
    return 2;
  else if(ref == Nucleotide::C && from == Nucleotide::T)
    return 2;
  else if(ref == Nucleotide::T && from == Nucleotide::C)
    return 2;
  return 0;
}

std::vector<KmerIsotope> generate_isotopes(const Nucleotides& kmer)
{
  constexpr static const Nucleotide NTisotopes[] = { Nucleotide::A, Nucleotide::C, Nucleotide::G, Nucleotide::T };
  constexpr static auto to_ascii = [](Nucleotide nt)
  { return (nt == Nucleotide::A ? 'A' : (nt == Nucleotide::C ? 'C' : (nt == Nucleotide::G ? 'G' : 'T'))); };

  std::vector<KmerIsotope> isotopes;
  isotopes.reserve(kmer.ntcount() * 3);
  for(std::size_t i = 0; i < kmer.ntcount(); ++i)
    for(auto& nt : NTisotopes)
      if(nt != kmer[i])
        isotopes.push_back({ i, to_ascii(nt), nt, score(nt, kmer[i])});
  return isotopes;
}

std::uint_fast64_t solidify_impl(std::size_t kmer_size, std::uint_fast64_t kmer_count, std::istream& is, const std::string& outname);

std::uint_fast64_t solidify(std::size_t kmer_size, std::uint_fast64_t kmer_count, std::istream& pis, const std::string& outname)
{
  /*
  std::ofstream os(outname);
  contig_writer cw(os, 80, kmer_size);
  read_fasta(pis) | [&cw](const std::string& read)
  {
    cw << read;
  };
  return kmer_count;
  */
  constexpr const static std::size_t min_kmer_cutoff = 16;
  std::vector<std::size_t> kmer_sizes;
  for(std::size_t k = kmer_size; k > min_kmer_cutoff; k -= 2)
    kmer_sizes.insert(kmer_sizes.begin(), k);
  namespace fs = std::filesystem;

  std::size_t min_s = kmer_count;

  std::istream* is = &pis;
  for(std::size_t i = kmer_sizes.size(); i --> 0; )
  {
    if(i < kmer_sizes.size() - 1)
      is = new std::ifstream(outname + std::to_string(kmer_sizes[i + 1]));
    auto k = kmer_sizes[i];
    std::string tmpfile = (i != 0 ? outname + std::to_string(k) : outname);

    std::ofstream tmpout(tmpfile);

    std::size_t tmps = solidify_impl(k, kmer_count, *is, tmpfile);
    min_s = std::min(min_s, tmps);

    if(i < kmer_sizes.size() - 1)
      delete is;
  }
  //cleanup
  for(std::size_t i = kmer_sizes.size(); i --> 0; )
  {
    std::filesystem::remove(std::filesystem::path(outname + std::to_string(kmer_sizes[i])));
  }
  return min_s;
}

std::uint_fast64_t solidify_impl(std::size_t kmer_size, std::uint_fast64_t kmer_count, std::istream& is, const std::string& outname)
{
  std::ofstream os(outname, std::ios::trunc);
  const double F = 1. / (bloom_params::Fc * kmer_size);
  const auto n = kmer_count;
  const double m = 1.44 * n * std::log2(1. / F);
                        // allocate at most 1GB
  const std::size_t tm = std::min(std::size_t(1 << 30),
                                  static_cast<std::size_t>(m + 0.5));
  counting_bloom_filter cb(tm, 0.7 * (tm / n));

  read_fasta(is)
    | [&cb, kmer_size](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;

      for(std::size_t i = 0; i <= read.size() - kmer_size; ++i)
        cb.add(Nucleotides::from_string(read.substr(i, kmer_size)));
    };
  contig_writer cw(os, 80, kmer_size);
  read_fasta(is)
    | [&cw, &cb, kmer_size, &kmer_count](const std::string& read)
    {
      if(read.size() < kmer_size)
        return;

      std::string rcpy = read;
      for(std::size_t i = 0; i <= read.size() - kmer_size; ++i)
      {
        auto tmp = rcpy.substr(i, kmer_size);
        auto kmer = Nucleotides::from_string(tmp);
        if(cb.count(kmer) >= max_occurence)
        {
          kmer_count -= read.size() - kmer_size + 1;
          return ; // skip reads containing kmers with very high abundance
        }
        else if(cb.count(kmer) <= min_occurence) // if H(v) < c:
        {
          auto bestCount = min_occurence - 1;
          short bestScore = -1;
          auto bestSeq = kmer;

          bool found = false;
          std::size_t best_pos = 0;
          auto isotopes = generate_isotopes(kmer);
          assert(!isotopes.empty());
          for(auto& el : isotopes)
          {
            auto kprime = kmer; kprime[el.pos] = el.nt;
            if(cb.count(kprime) > bestCount || (cb.count(kprime) == bestCount && el.score > bestScore))
            {
              bestSeq = kprime;
              bestCount = cb.count(kprime);
              best_pos = el.pos;
              bestScore = el.score;
              found = true;
            }
          }
          if(found)
          {
            rcpy = rcpy.substr(0, i) + bestSeq.to_string() + rcpy.substr(i + kmer.ntcount());
            i += best_pos;
          }
        }
      }
      cw << rcpy;
    };
  return kmer_count;
}

