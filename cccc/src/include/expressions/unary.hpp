#pragma once

#include <expressions/expression.hpp>
#include <types/type.hpp>
#include <lex/token.hpp>

namespace mhc4
{

namespace expressions
{

expression::ptr unary(token tok, bool postfix, expression::ptr operand);
expression::ptr unary(token tok, bool postfix, type::ptr type);

}

}
