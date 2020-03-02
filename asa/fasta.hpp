#pragma once

#include <iostream>
#include <string>

template<typename Fn>
void read_fasta(std::istream& reads, Fn&& on_read_callback)
{
  std::string current_read;
  for(std::string line; std::getline(reads, line); )
  {
    if(line.front() == '>')
    {
      if(!current_read.empty())    // note: this has to be a nested if
      {
        on_read_callback(current_read);
        current_read.clear();
      }
    }
    else
      current_read += line;
  }
  if(!current_read.empty())
    on_read_callback(current_read);
  // reset stream
  reads.clear(); reads.seekg(0, std::ios::beg);
}

struct seq_stream
{
  std::istream& reads;
};

inline seq_stream read_fasta(std::istream& reads)
{ return seq_stream { reads }; }

template<typename F>
inline void operator|(seq_stream&& s, F&& on_read_callback)
{ read_fasta(s.reads, on_read_callback); }

