#include <statements/expression.hpp>

namespace mhc4
{

namespace statements
{

_expression::_expression(const source_location& loc, expression::ptr expr)
    : statement_semicolon_printer(loc), expr(std::move(expr))
{  }

void _expression::print(std::size_t) const
{
    expr->print_expr();
}

void _expression::infer(semantics::scope& s) const
{
    expr->infer(s);
}

void _expression::generate_ir(irconstructer& irc, IRScope& irscope)
{
    expr->make_rvalue(irc, irscope);
}

}

}
