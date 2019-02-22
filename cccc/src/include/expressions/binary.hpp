#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

#include <stdexcept>

namespace mhc4
{

class unknown_binary_op_exception : public std::invalid_argument
{
public:
    unknown_binary_op_exception(token t)
        : invalid_argument("Binary operator " + t.userdata() + " is not supported")
    {  }
};

namespace expressions
{

expression::ptr binary(token tok, expression::ptr left, expression::ptr right);

}

}
