#pragma once

#include "nucleotides.hpp"
#include <string>
#include <vector>

namespace bloom_params
{
/*  1 << 20   -   1 Megabyte
 *  1 << 21   -   2 Megabyte
 *  1 << 24   -  16 Megabyte
 */
constexpr static const double Fc = 16. / 2.08; // <- wanted false positive rate factor, false positive rate is F = (Fc * k)^-1
}

struct bloom_filter
{
  // m - number of bits in the table (not bytes, we have vector<bool>!)
  // k - number of hash functions to use
  //    optimal k is (m/n)ln 2, n is number of elements to be inserted
  bloom_filter(std::size_t m, std::size_t k)
    : k(k), table(m, false)
  {  }

  void add(const Nucleotides& text);
  bool contains(const Nucleotides& text) const;
private:
  std::size_t k;
  std::vector<bool> table;
};

// based on Quingpeng et al. (2014)
struct counting_bloom_filter
{
  // m - number of bits in the table
  // k - number of hash functions to use
  //    optimal k is (m/n)ln 2, n is number of elements to be inserted
  counting_bloom_filter(std::size_t m, std::size_t k)
    : k(k), table(m, 0)
  {  }

  void add(const Nucleotides& text);
  unsigned short count(const Nucleotides& text) const;
private:
  std::size_t k;
  std::vector<unsigned short> table;
};

