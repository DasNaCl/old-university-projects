#include <ast/binary_printer.hpp>

#include <iostream>

namespace mhc4
{

namespace ast
{

binary_printer::binary_printer(token op, expression::ptr lhs, expression::ptr rhs)
    : expression(op.get_location()), op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
{  }

void binary_printer::print() const
{
    lhs->print_expr();
    std::cout << " " << op.userdata() << " ";
    rhs->print_expr();
}

bool binary_printer::is_l_value() const noexcept
{
    // we are always an rvalue
    return false;
}

}

}
