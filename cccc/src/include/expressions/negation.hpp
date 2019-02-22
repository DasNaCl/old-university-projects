#pragma once

#include <ast/unary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class negation : public ast::unary_printer
{
public:
    explicit negation(token op, bool postfix, expression::ptr operand);

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
    void make_cf(irconstructer& irc, IRScope& irscope, llvm::BasicBlock* trueBB, llvm::BasicBlock* falseBB) override;
};

}

}
