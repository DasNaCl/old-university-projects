#include "contig_writer.hpp"
#include "nucleotides.hpp"

#include <iostream>
                                // order defined in nucleotides.hpp
constexpr static const char to_ascii[] = { 'A', 'C', 'T', 'G' };

contig_writer& operator<<(contig_writer& cw, const Nucleotides& contig)
{
  // skip way too small contigs
  if(cw.k > contig.ntcount())
    return cw;
  cw.os << ">" << (++cw.contig_count) << "\n";
  for(std::size_t i = 0; i < contig.ntcount(); i += cw.w)
  {
    for(std::size_t j = i; j < i + cw.w && j < contig.ntcount(); ++j)
      cw.os << to_ascii[static_cast<std::uint_fast8_t>(contig[j])];
    cw.os << "\n";
  }
  return cw;
}

contig_writer& operator<<(contig_writer& cw, const std::string& contig)
{
  // skip way too small contigs
  if(cw.k > contig.size())
    return cw;
  cw.os << ">" << (++cw.contig_count) << "\n";
  for(std::size_t i = 0, j = 0; i < contig.size(); i += cw.w, ++j)
  {
    cw.os.write(contig.data() + j * cw.w, std::min(contig.size() - j * cw.w, cw.w));
    cw.os << "\n";
  }
  return cw;
}

