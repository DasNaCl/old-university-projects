#pragma once

#include <ast/binary_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

class arithmetic : public ast::binary_printer
{
public:
    explicit arithmetic(token tok, expression::ptr left, expression::ptr right);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_rvalue(irconstructer& ir, IRScope& irscope) override;
};

}

}

