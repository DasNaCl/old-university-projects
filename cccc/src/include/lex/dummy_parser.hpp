#pragma once

#include <basic_parser.hpp>

#include <lex/lexer.hpp>

namespace mhc4
{

//This "parser" is for printing the tokens, only
class dummy_parser : public basic_parser
{
public:
    explicit dummy_parser(lexer& lex);
    ~dummy_parser() override {}

    void parse() override;
};

}
