#pragma once

#include <expressions/expression.hpp>

#include <types/type.hpp>

#include <lex/token.hpp>

namespace mhc4
{

namespace expressions
{

class cast_expressions_not_supported_exception : public std::invalid_argument
{
public:
    cast_expressions_not_supported_exception()
        : invalid_argument("Cast expressions are not (yet) supported by this parser")
    {  }
};

expression::ptr cast(token tok, type::ptr typ, expression::ptr operand);

}

}
