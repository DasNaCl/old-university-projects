#pragma once

#include <ast/binary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class inequality : public ast::binary_printer
{
public:
    explicit inequality(token tok, expression::ptr left, expression::ptr right);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;
};

}

}
