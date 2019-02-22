#include <lex/dummy_parser.hpp>

#include <iostream>

namespace mhc4
{

dummy_parser::dummy_parser(lexer& lex) : basic_parser(lex)
{  }

void dummy_parser::parse()
{
    std::ios::sync_with_stdio(false);

    /*
    for(token tok = lex.lex(); !tok.is_kind_of(tok::end_of_file); tok = lex.lex())
        std::cout << tok.to_string() << "\n";
    */

    lex.print_all();

    std::ios::sync_with_stdio(true);
}

}
