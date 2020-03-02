#include "nucleotides.hpp"

#include <cassert>
#include <limits>

constexpr static const std::size_t el_s = 64 / 2;
constexpr static const std::size_t bits = std::numeric_limits<std::uint_fast64_t>::digits;

// risk getting 0 points by making the bold assumption that the test machine has a uint_fast64_t
// type with exactly 64 bits
static_assert(bits == 64, "The code assumes that your machine has 8 bits per byte and that sizeof(std::uint_fast64_t) == 8.");

// see graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
inline std::uint_fast64_t reverse_complement_bucket(std::uint_fast64_t v, std::size_t nt_count)
{
  v = ((v >> 2)  & 0x3333333333333333) | ((v & 0x3333333333333333) << 2);
  v = ((v >> 4)  & 0x0F0F0F0F0F0F0F0F) | ((v & 0x0F0F0F0F0F0F0F0F) << 4);
  v = ((v >> 8)  & 0x00FF00FF00FF00FF) | ((v & 0x00FF00FF00FF00FF) << 8);
  v = ((v >> 16) & 0x0000FFFF0000FFFF) | ((v & 0x0000FFFF0000FFFF) << 16);
  v = ((v >> 32) & 0x00000000FFFFFFFF) | ((v & 0x00000000FFFFFFFF) << 32);

  v = v ^ 0xAAAAAAAAAAAAAAAA;

  return v >> (2 * (el_s - nt_count));
}

Nucleotides::Nucleotides(std::size_t count)
  : data((count == 0 ? 0 : (count - 1) / el_s) + 1, 0ULL), count(count)
{  }

std::size_t Nucleotides::ntcount() const
{
  return count;
}

Nucleotides Nucleotides::from_string(const std::string& str)
{
  Nucleotides nts;
  for(auto a : str)
    nts.append(a);
  return nts;
}

Nucleotides Nucleotides::tail() const
{
  Nucleotides t;
  for(auto it = begin() + 1; it != end(); ++it)
    t.append(*it);
  return t;
}

Nucleotides Nucleotides::front_tail() const
{
  Nucleotides t;
  for(auto it = begin(); it + 1 != end(); ++it)
    t.append(*it);
  return t;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Nucleotides& Nucleotides::operator<<=(std::size_t amount)
{
  assert(amount == 1);

  if(data.size() == 1)
  {
    const std::uint_fast8_t storage = (data[0] >> (2 * (el_s - 1))) & 0b11;

    data[0] <<= 2; // <- we shift in bits!
    ++count;

    if(count > el_s)
    {
      data.emplace_back();
      data[1] |= storage;
    }
  }
  else
  {
    std::uint_fast8_t storage = '\0';
    for(std::size_t i = 0; i < data.size(); ++i)
    {
      std::uint_fast8_t new_storage = (data[i] >> (2 * (el_s - 1))) & 0b11;
      data[i] <<= 2;
      
      if(i > 0)
        data[i] |= storage;

      storage = new_storage;
    }
    ++count;
    if(count > el_s * data.size())
      data.push_back(storage);
    // clear bits to allow for faster/easier comparison of two Nucleotides
    if(count < el_s * data.size())
    {
      assert(el_s * data.size() - count <= el_s);

      const std::uint_fast64_t mask = (static_cast<std::uint_fast64_t>(-1)) >> (bits - (2 * (count % bits)));
      
      data.back() &= mask; // mask looks like 0b000011111...11
    }
  }
  return *this;
}

Nucleotides& Nucleotides::operator>>=(std::size_t amount)
{
  assert(amount == 1);

  if(data.size() == 1)
  {
    data[0] >>= 2; // <- we shift in bits!
    if(count > 0)
      --count;
  }
  else
  {
    char storage = '\0';
    for(std::size_t i = data.size(); i --> 0; )
    {
      char new_storage = data[i] & 0b11;
      data[i] >>= 2;
      
      if(i < data.size() - 1)
        data[i] |= static_cast<std::uint_fast64_t>(storage) << (2 * (el_s - 1));

      storage = new_storage;
    }
    if(count > 0)
    {
      bool was_bigger = count > el_s * (data.size() - 1);

      --count;

      if(was_bigger && count == el_s * (data.size() - 1))
        data.pop_back();
    }
    // clear bits to allow for faster/easier comparison of two Nucleotides
    if(count < el_s * data.size())
    {
      assert(el_s * data.size() - count <= el_s);

      const std::uint_fast64_t mask = (static_cast<std::uint_fast64_t>(-1)) >> (bits - (2 * (count % bits)));
      
      data.back() &= mask; // mask looks like 0b000011111...11
    }
  }
  return *this;
}
#pragma GCC diagnostic pop

Nucleotide Nucleotides::get(std::size_t pos) const
{
  assert(pos < count && count > 0);

  const std::uint_fast64_t& bucket = data[(count - pos - 1) / el_s];
  const std::size_t base_index = (count - pos - 1) % el_s;

  std::uint_fast8_t cnt = (bucket >> (2 * base_index)) & 0b11;

  return static_cast<Nucleotide>(cnt);
}

Nucleotides::NucleotideProxy<false> Nucleotides::get(std::size_t pos)
{
  return NucleotideProxy<false>(*this, pos);
}

void Nucleotides::append(Nucleotide nt)
{
  const std::uint_fast64_t cnt = static_cast<std::uint_fast8_t>(nt);

  *this <<= 1;

  data[0] |= cnt;
}

void Nucleotides::append(char nt)
{
  assert(nt == 'A' || nt == 'G' || nt == 'T' || nt == 'C');

  append(static_cast<Nucleotide>((nt >> 1) & 3));
}

void Nucleotides::prepend(Nucleotide nt)
{
  const std::uint_fast64_t cnt = static_cast<std::uint_fast8_t>(nt);

  ++count;

  if(count > data.size() * el_s)
    data.emplace_back();

  std::uint_fast64_t& bucket = data[(count - 1) / el_s];
  const std::size_t base_index = (count - 1) % el_s;

  bucket |= cnt << (2 * base_index);
}

void Nucleotides::prepend(char nt)
{
  assert(nt == 'A' || nt == 'G' || nt == 'T' || nt == 'C');

  prepend(static_cast<Nucleotide>((nt >> 1) & 3));
}

Nucleotides& Nucleotides::operator+=(Nucleotide nt)
{
  append(nt);
  return *this;
}

Nucleotides& Nucleotides::operator+=(char nt)
{
  append(nt);
  return *this;
}

void Nucleotides::set(std::size_t pos, char nt)
{
  assert(nt == 'A' || nt == 'G' || nt == 'T' || nt == 'C');

  set(pos, static_cast<Nucleotide>((nt >> 1) & 3));
}

void Nucleotides::set(std::size_t pos, Nucleotide nt)
{
  assert(pos < count && count > 0);

  const std::uint_fast64_t cnt = static_cast<unsigned char>(nt);

  std::uint_fast64_t& bucket = data[(count - pos - 1) / el_s];
  const std::size_t base_index = (count - pos - 1) % el_s;

  //first clear the bits
  bucket &= ~(0b11 << (2 * base_index));

  // then set the bits to the new nucleotide
  bucket |= cnt << (2 * base_index);
}

Nucleotides Nucleotides::twin() const
{
  if(data.size() > 1)
  {
    // just do it expensively, since the efficient implementation below is bugged for e.g. 5 buckets or more
    auto str = to_string();
    for(auto& c : str)
      switch(c)
      {
      case 'A': c = 'T'; break;
      case 'C': c = 'G'; break;
      case 'G': c = 'C'; break;
      case 'T': c = 'A'; break;
      }
    return Nucleotides::from_string(std::string(str.rbegin(), str.rend()));
  }
  Nucleotides tmp(count);

  assert(tmp.data.size() == data.size());
  for(std::size_t f = 0; f < data.size(); ++f)
    tmp.data[f] = reverse_complement_bucket(data[data.size() - f - 1],
                                            data.size() - f - 1 > 0 ? el_s : count % el_s);

  return tmp;
}

std::string Nucleotides::to_string() const
{
  std::string str;
  str.reserve(count);
  for(std::size_t i = 0; i < count; ++i)
  {
    switch(get(i))
    {
    default: assert(false);
    case Nucleotide::A: str.push_back('A'); break;
    case Nucleotide::C: str.push_back('C'); break;
    case Nucleotide::G: str.push_back('G'); break;
    case Nucleotide::T: str.push_back('T'); break;
    }
  }
  return str;
}

std::string Nucleotides::to_string(Dir dir) const
{
  return (dir == Dir::Forward ? to_string() : twin().to_string());
}

bool operator==(const Nucleotides& lhs, const Nucleotides& rhs)
{
  if(lhs.ntcount() != rhs.ntcount())
    return false;

  for(auto it0 = lhs.buckets_cbegin(), it1 = rhs.buckets_cbegin(); it0 != lhs.buckets_cend() && it1 != rhs.buckets_cend(); ++it0, ++it1)
  {
    if(*it0 != * it1)
      return false;
  }
  return true;
}

bool operator!=(const Nucleotides& lhs, const Nucleotides& rhs)
{
  return !(lhs == rhs);
}

Nucleotides operator+(Nucleotide nt, const Nucleotides& nts)
{
  Nucleotides new_nts = nts;
  
  new_nts.prepend(nt);

  return new_nts;
}

Nucleotides operator+(const Nucleotides& nts, Nucleotide nt)
{
  Nucleotides new_nts = nts;
  
  new_nts.append(nt);

  return new_nts;
}

Nucleotides operator+(char nt, const Nucleotides& nts)
{
  Nucleotides new_nts = nts;
  
  new_nts.append(nt);

  return new_nts;
}

Nucleotides operator+(const Nucleotides& nts, char nt)
{
  Nucleotides new_nts = nts;
  
  new_nts.append(nt);

  return new_nts;
}
 
bool operator<=(const Nucleotides& lhs, const Nucleotides& rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::less_equal<Nucleotide>());
}

bool operator>=(const Nucleotides& lhs, const Nucleotides& rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater_equal<Nucleotide>());
}

bool operator< (const Nucleotides& lhs, const Nucleotides& rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::less<Nucleotide>());
}

bool operator> (const Nucleotides& lhs, const Nucleotides& rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater<Nucleotide>());
}

