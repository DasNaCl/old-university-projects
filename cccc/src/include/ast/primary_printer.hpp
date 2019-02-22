#pragma once

#include <expressions/expression.hpp>

#include <lex/token.hpp>

namespace mhc4
{

namespace ast
{

class primary_printer : public expression
{
protected:
    explicit primary_printer(token tok);
public:
    void print_expr() const override;
protected:
    void print() const override;
protected:
    token tok;
};

}

}
