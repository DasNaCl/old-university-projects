#include <ast/statement_semicolon_printer.hpp>

#include <iostream>

namespace mhc4
{

namespace ast
{

statement_semicolon_printer::statement_semicolon_printer(const source_location& loc)
    : statement(loc)
{  }

void statement_semicolon_printer::print_statement(std::size_t depth) const
{
    for(std::size_t d = depth; d > 0; --d)
        std::cout << "\t";
    print(depth);
    std::cout << ";\n";
}

}

}
