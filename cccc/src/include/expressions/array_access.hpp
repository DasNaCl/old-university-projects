#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

namespace mhc4
{

namespace expressions
{

class array_access : public expression
{
public:
    explicit array_access(token tok, expression::ptr from, expression::ptr at);

    type::ptr _infer(semantics::scope& s) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
    llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) override;

    void print() const override;
    bool is_l_value() const noexcept override;
private:
    std::pair<llvm::Value*, llvm::Value*> make_value(irconstructer& irc, IRScope& irscope);
private:
    token op;
    expression::ptr from;
    expression::ptr at;
};

}

}
