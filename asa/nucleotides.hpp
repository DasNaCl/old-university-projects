#pragma once

#include "nucleotides.hpp"

#include <iterator>
#include <cstdint>
#include <string>
#include <vector>


enum Nucleotide
{
  A = 0b00,    // == (('A' >> 1) & 3)   // complement is T
  C = 0b01,    // == (('C' >> 1) & 3)   // complement is G
  G = 0b11,    // == (('G' >> 1) & 3)   // complement is C
  T = 0b10     // == (('T' >> 1) & 3)   // complement is A
};

enum Dir : char
{
  Forward  =  1,
  Backward = -1
};

class Nucleotides
{
public:
  template<bool is_const>
  struct NucleotideProxy
  {
    using T = std::conditional_t<is_const, const Nucleotides, Nucleotides>;

    NucleotideProxy(T& nts, std::size_t pos)
      : nts(nts), pos(pos)
    { }

    operator Nucleotide() const { const Nucleotides& cnts = nts; return cnts.get(pos); }

    // make return type const to prevent code like `nts[0] = nts[1] = 'C';`
    Nucleotide operator=(char nt)
    {
      if constexpr(is_const)
      {
        static_assert(!is_const);
        
        return Nucleotide::A;
      }
      else
      {
        nts.set(pos, nt);
        const auto& cnts = nts;

        return cnts.get(pos);
      }
    }

    Nucleotide operator=(Nucleotide nt)
    {
      if constexpr(is_const)
      {
        static_assert(!is_const);
        
        return Nucleotide::A;
      }
      else
      {
        nts.set(pos, nt);
        const auto& cnts = nts;

        return cnts.get(pos);
      }
    }


    Nucleotide operator*() { const Nucleotides& cnts = nts; return cnts.get(pos); } // sorry Dijkstra, I know you don't like this
  private:
    T& nts;
    std::size_t pos;
  };
public:
  explicit Nucleotides(std::size_t count = 0);

  static Nucleotides from_string(const std::string& str);

  Nucleotides tail() const;
  Nucleotides front_tail() const;
  void clear()
  {
    data.clear(); data.emplace_back();
    count = 0;
  }

  bool empty() const
  { return count == 0; }

  // shifting left implicitly adds 'A' to the chain
  Nucleotides& operator<<=(std::size_t amount);
  Nucleotides& operator>>=(std::size_t amount);

  std::size_t ntcount() const;
  NucleotideProxy<false> get(std::size_t pos);
  Nucleotide get(std::size_t pos) const;

  NucleotideProxy<false> operator[](std::size_t pos)  { return get(pos); }
  Nucleotide operator[](std::size_t pos) const { return get(pos); }

  void append(Nucleotide nt);
  void append(char nt);
  void prepend(Nucleotide nt);
  void prepend(char nt);

  Nucleotides& operator+=(const Nucleotides& nts);
  Nucleotides& operator+=(Nucleotide nt);
  Nucleotides& operator+=(char nt);

  void set(std::size_t pos, char nt);
  void set(std::size_t pos, Nucleotide nt);

  Nucleotides twin() const;

  std::string to_string() const;
  std::string to_string(Dir dir) const;

  std::vector<std::uint_fast64_t>::const_iterator buckets_cbegin() const { return data.cbegin(); }
  std::vector<std::uint_fast64_t>::const_iterator buckets_cend() const { return data.cend(); }

  template<bool is_const>
  struct nucleotides_iterator
  {
    using T = std::conditional_t<is_const, const Nucleotides, Nucleotides>;

    nucleotides_iterator(T& nts, std::size_t pos)
      : nts(&nts), pos(pos)
    {  }

    nucleotides_iterator& operator=(const nucleotides_iterator& ntsi)
    { nts = ntsi.nts; pos = ntsi.pos; return *this; }

    nucleotides_iterator& operator++()
    { ++pos; return *this; }

    nucleotides_iterator operator++(int)
    { nucleotides_iterator cpy = *this; ++pos; return cpy; }

    nucleotides_iterator& operator--()
    { --pos; return *this; }

    nucleotides_iterator operator--(int)
    { nucleotides_iterator cpy = *this; --pos; return cpy; }

    NucleotideProxy<is_const> operator*() const
    { return NucleotideProxy<is_const>(*nts, pos); }

    nucleotides_iterator& operator-=(std::size_t amount)
    {
      pos -= amount;
      return *this;
    }
    nucleotides_iterator& operator+=(std::size_t amount)
    {
      pos += amount;
      return *this;
    }

    template<bool consty>
    friend bool operator==(const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);
    template<bool consty>
    friend bool operator!=(const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);
    template<bool consty>
    friend bool operator<=(const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);
    template<bool consty>
    friend bool operator>=(const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);
    template<bool consty>
    friend bool operator< (const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);
    template<bool consty>
    friend bool operator> (const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);

    template<bool consty>
    friend nucleotides_iterator<consty> operator+(const nucleotides_iterator<consty>& lhs, std::size_t amount);
    template<bool consty>
    friend nucleotides_iterator<consty> operator+(std::size_t amount, const nucleotides_iterator<consty>& rhs);

    template<bool consty>
    friend nucleotides_iterator<consty> operator-(const nucleotides_iterator<consty>& lhs, std::size_t amount);
    template<bool consty>
    friend std::size_t operator-(const nucleotides_iterator<consty>& lhs, const nucleotides_iterator<consty>& rhs);

    template<bool consty>
    friend void swap(nucleotides_iterator<consty>& lhs, nucleotides_iterator<consty>& rhs);
  private:
    T* nts;
    std::size_t pos;
  };

  nucleotides_iterator<false> begin() { return nucleotides_iterator<false>(*this, 0); }
  nucleotides_iterator<false> end() { return nucleotides_iterator<false>(*this, count); }
  nucleotides_iterator<true> begin() const { return nucleotides_iterator<true>(*this, 0); }
  nucleotides_iterator<true> end() const { return nucleotides_iterator<true>(*this, count); }
private:
  std::vector<std::uint_fast64_t> data;
  std::size_t count;
};

struct NucleotidesHasher
{
  std::size_t operator()(const Nucleotides& key) const; // <- defined in bloom_filter.cpp for maximum obscurity
};

Nucleotides operator+(Nucleotide nt, const Nucleotides& nts);
Nucleotides operator+(const Nucleotides& nts, Nucleotide nt);

Nucleotides operator+(char nt, const Nucleotides& nts);
Nucleotides operator+(const Nucleotides& nts, char nt);

bool operator==(const Nucleotides& lhs, const Nucleotides& rhs);
bool operator!=(const Nucleotides& lhs, const Nucleotides& rhs);
bool operator<=(const Nucleotides& lhs, const Nucleotides& rhs);
bool operator>=(const Nucleotides& lhs, const Nucleotides& rhs);
bool operator< (const Nucleotides& lhs, const Nucleotides& rhs);
bool operator> (const Nucleotides& lhs, const Nucleotides& rhs);

template<>
struct std::iterator_traits<Nucleotides::nucleotides_iterator<false>>
{
  typedef std::size_t difference_type;
  typedef Nucleotide value_type;
  typedef Nucleotides::NucleotideProxy<false> reference;
  typedef Nucleotides::NucleotideProxy<false> pointer;
  typedef std::random_access_iterator_tag iterator_category;
};

template<>
struct std::iterator_traits<Nucleotides::nucleotides_iterator<true>>
{
  typedef std::size_t difference_type;
  typedef const Nucleotide value_type;
  typedef Nucleotides::NucleotideProxy<true> reference;
  typedef Nucleotides::NucleotideProxy<true> pointer;
  typedef std::random_access_iterator_tag iterator_category;
};


template<bool is_const>
bool operator==(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  if(lhs.nts != rhs.nts)
    return false;
  if(lhs.pos >= lhs.nts->ntcount() && rhs.pos >= lhs.nts->ntcount())
    return true;
  return lhs.pos == rhs.pos;
}

template<bool is_const>
bool operator!=(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{ return !(lhs == rhs); }

template<bool is_const>
bool operator<=(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  if(lhs.nts != rhs.nts)  // note: this is a bit "suspicious", since for us here, !(lhs <= rhs && rhs <= lhs) can be true,
    return false;         //       although lhs != rhs
  if(rhs.pos >= lhs.nts->ntcount())
    return true;
  return lhs.pos <= rhs.pos;
}

template<bool is_const>
bool operator>=(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  if(lhs.nts != rhs.nts)  // note: this is a bit "suspicious", since for us here, !(lhs >= rhs && rhs >= lhs) can be true,
    return false;         //       although lhs != rhs
  if(lhs.pos >= lhs.nts->ntcount())
    return true;
  return lhs.pos >= rhs.pos;
}

template<bool is_const>
bool operator<(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  if(lhs.nts != rhs.nts)
    return false;
  if(rhs.pos >= lhs.nts->ntcount() && lhs.pos < lhs.nts->ntcount())
    return true;
  return lhs.pos < rhs.pos;
}

template<bool is_const>
bool operator>(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  if(lhs.nts != rhs.nts)
    return false;
  if(lhs.pos >= lhs.nts->ntcount() && rhs.pos < rhs.nts->ntcount())
    return true;
  return lhs.pos > rhs.pos;
}

template<bool is_const>
Nucleotides::nucleotides_iterator<is_const> operator+(const Nucleotides::nucleotides_iterator<is_const>& lhs, std::size_t amount)
{
  Nucleotides::nucleotides_iterator<is_const> cpy = lhs;
  while(amount --> 0)
    ++cpy;
  return cpy;
}

template<bool is_const>
Nucleotides::nucleotides_iterator<is_const> operator+(std::size_t amount, const Nucleotides::nucleotides_iterator<is_const>& lhs)
{
  Nucleotides::nucleotides_iterator<is_const> cpy = lhs;
  while(amount --> 0)
    ++cpy;
  return cpy;
}

template<bool is_const>
Nucleotides::nucleotides_iterator<is_const> operator-(const Nucleotides::nucleotides_iterator<is_const>& lhs, std::size_t amount)
{
  Nucleotides::nucleotides_iterator<is_const> cpy = lhs;
  while(amount --> 0)
    --cpy;
  return cpy;
}

template<bool is_const>
std::size_t operator-(const Nucleotides::nucleotides_iterator<is_const>& lhs, const Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  return lhs.pos - rhs.pos;
}

template<bool is_const>
void swap(Nucleotides::nucleotides_iterator<is_const>& lhs, Nucleotides::nucleotides_iterator<is_const>& rhs)
{
  std::swap(lhs.nts, rhs.nts);
  std::swap(lhs.pos, rhs.pos);
}


inline Dir flip(Dir d)
{ return static_cast<Dir>(-static_cast<char>(d)); }

struct Node
{
  Dir direction;
  Nucleotides ncs;
};
inline bool operator<(const Node& lhs, const Node& rhs)
{ return lhs.ncs < rhs.ncs; }
inline bool operator==(const Node& lhs, const Node& rhs)
{ return lhs.ncs == rhs.ncs; }

struct Edge
{
  Node from;
  Node to;

  Nucleotide nt;

  Dir dir;
};

