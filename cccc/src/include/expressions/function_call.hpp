#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

#include <vector>

namespace mhc4
{

namespace expressions
{

class function_call : public expression
{
public:
    explicit function_call(token tok, expression::ptr callee, std::vector<expression::ptr> args);

    type::ptr _infer(semantics::scope& s) override;
    void print() const override;

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    bool is_l_value() const noexcept override;
private:
    token tok;
    expression::ptr callee;
    std::vector<expression::ptr> args;
};

}

}

