#include "bloom_filter.hpp"
#include <cstdint>
#include <limits>

inline std::uint_fast64_t murmur3(std::uint_fast64_t h)
{
  h ^= h >> 33ull;
  h *= 0xff51afd7ed558ccdull;
  h ^= h >> 33ull;
  h *= 0xc4ceb9fe1a85ec53ull;
  h ^= h >> 33ull;
  return h;
}

inline std::uint_fast32_t nthHash(std::size_t k, std::uint_fast32_t hashA, std::uint_fast32_t hashB, std::uint_fast32_t n)
{
  return (hashA + k * hashB) % n;
}

inline std::size_t position(std::size_t size, std::uint_fast64_t hash, std::size_t i)
{
  return nthHash(i, static_cast<std::uint_fast32_t>((hash & 0xFFFFFFFF00000000ull) >> 32ull),
                    static_cast<std::uint_fast32_t>(hash & 0xFFFFFFFFull), size);
}

inline std::uint_fast64_t hash(const std::string& text)
{
  std::uint_fast64_t h = 0;
  for(auto& c : text)
    h ^= c + 0x9e3779b9 + (h << 6) + (h >> 2);  // <- boost hash_combine: stackoverflow.com/questions/35985960
  return h;
}
inline std::uint_fast64_t hash(const Nucleotides& text)
{
  std::uint_fast64_t h = 0;
  for(auto it = text.buckets_cbegin(); it != text.buckets_cend(); ++it)
    h ^= murmur3(*it) + 0x9e3779b9 + (h << 6) + (h >> 2);  // <- boost hash_combine: stackoverflow.com/questions/35985960
  return h;
}

std::size_t NucleotidesHasher::operator()(const Nucleotides& key) const
{ return hash(key); }

void bloom_filter::add(const Nucleotides& ncs)
{
  const std::uint_fast64_t h = hash(ncs);
  for(std::size_t i = 0; i < k; ++i)
    table[position(table.size(), h, i)] = true;
}

bool bloom_filter::contains(const Nucleotides& ncs) const
{
  const std::uint_fast64_t h = hash(ncs);
  for(std::size_t i = 0; i < k; ++i)
    if(!table[position(table.size(), h, i)])
      return false;
  return true;
}

void counting_bloom_filter::add(const Nucleotides& text)
{
  const std::uint_fast64_t h = hash(text);
  for(std::size_t i = 0; i < k; ++i)
  {
    auto& val = table[position(table.size(), h, i)];
    if(val < std::numeric_limits<unsigned short>::max())
      val = val + 1;
  }
}

unsigned short counting_bloom_filter::count(const Nucleotides& text) const
{
  unsigned short count = std::numeric_limits<unsigned short>::max();
  const std::uint_fast64_t h = hash(text);
  for(std::size_t i = 0; i < k; ++i)
    count = std::min(count, table[position(table.size(), h, i)]);
  return count;
}

