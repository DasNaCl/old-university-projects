#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

namespace mhc4
{

namespace expressions
{

class condition : public expression
{
public:
    explicit condition(token tok, expression::ptr expr, expression::ptr consequence, expression::ptr alternative);

    type::ptr _infer(semantics::scope& s) override;

    void make_cf(irconstructer& irc, IRScope& irscope,
                 llvm::BasicBlock* trueBB,
                 llvm::BasicBlock* falseBB) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
    void print() const override;

    bool is_l_value() const noexcept override;
private:
    token tok;
    expression::ptr expr;
    expression::ptr consequence;
    expression::ptr alternative;
};

}

}
