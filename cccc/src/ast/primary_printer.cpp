#include <ast/primary_printer.hpp>

#include <iostream>

namespace mhc4
{

namespace ast
{

primary_printer::primary_printer(token tok)
    : expression(tok.get_location()), tok(tok)
{  }

void primary_printer::print_expr() const
{
    // do not print as `( expr )`

    print();
}

void primary_printer::print() const
{
    std::cout << tok.userdata();
}

}

}
