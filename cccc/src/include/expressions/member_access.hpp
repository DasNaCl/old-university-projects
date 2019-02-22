#pragma once

#include <expressions/expression.hpp>
#include <lex/token.hpp>

#include <sema/analyzer.hpp>

#include <vector>
#include <deque>

namespace mhc4
{

namespace expressions
{

class member_access : public expression
{
public:
    explicit member_access(token tok, expression::ptr from, expression::ptr to);

    type::ptr _infer(semantics::scope& s) override;
    void print() const override;

    llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    bool is_l_value() const noexcept override;
private:
    type::ptr struct_definition(semantics::scope& s);
    token name_of_struct_member() const;
    std::pair<llvm::Value*, llvm::Value*> make_value(irconstructer& irc, IRScope& irscope);
private:
    token tok;
    expression::ptr from_expr;
    expression::ptr to_expr;

    std::size_t pos_in_struct;

    std::deque<semantics::scope> deq;
};

}

}

