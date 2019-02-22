#pragma once

#include <ast/binary_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

class assign : public ast::binary_printer
{
public:
    explicit assign(token tok, expression::ptr left, expression::ptr right);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
private:
    std::pair<llvm::Value*, llvm::Value*> make_value(irconstructer& irc, IRScope& irscope);

};

}

}
