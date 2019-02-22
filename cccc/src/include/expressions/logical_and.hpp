#pragma once

#include <ast/binary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class logical_and : public ast::binary_printer
{
public:
    explicit logical_and(token tok, expression::ptr left, expression::ptr right);

    type::ptr _infer(semantics::scope& s) override;

    void make_cf(irconstructer& irc, IRScope& irscope,
                 llvm::BasicBlock* trueBB,
                 llvm::BasicBlock* falseBB) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
};

}

}
