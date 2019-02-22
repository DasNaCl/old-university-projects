#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace ast
{

class unary_printer : public expression
{
protected:
    explicit unary_printer(token op, bool postfix, expression::ptr operand);

    type::ptr _infer(semantics::scope& s) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
    llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) override;

    void print() const override;
    bool is_l_value() const noexcept override;
protected:
    token op;
    bool postfix;
    expression::ptr operand;
};

}

}
