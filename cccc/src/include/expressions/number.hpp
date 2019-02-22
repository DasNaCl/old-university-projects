#pragma once

#include <ast/primary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class number : public ast::primary_printer
{
public:
    explicit number(token tok);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    bool is_primary() const noexcept override;
    bool is_null() const noexcept override;

    bool is_l_value() const noexcept override;
};

}

}
