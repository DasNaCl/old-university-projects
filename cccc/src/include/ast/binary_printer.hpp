#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

namespace mhc4
{

namespace ast
{

class binary_printer : public expression
{
protected:
    explicit binary_printer(token op, expression::ptr lhs, expression::ptr rhs);

    void print() const override;

    bool is_l_value() const noexcept override;
protected:
    token op;
    expression::ptr lhs;
    expression::ptr rhs;
};

}

}
