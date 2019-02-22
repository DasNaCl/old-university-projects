#pragma once

#include <ast/binary_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

class comparison : public ast::binary_printer
{
public:
    explicit comparison(token tok, expression::ptr left, expression::ptr right);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
};

}

}
