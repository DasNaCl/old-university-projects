#pragma once

#include <lex/lexer.hpp>

#include <memory>

namespace mhc4
{

class basic_parser
{
public:
    using ptr = std::unique_ptr<basic_parser>;
public:
    basic_parser(lexer& lex) : lex(lex)
        {  }
    virtual ~basic_parser() = default;

    virtual void parse() = 0;
protected:
    lexer& lex;
};

}
