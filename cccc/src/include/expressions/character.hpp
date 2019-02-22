#pragma once

#include <ast/primary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class character : public ast::primary_printer
{
public:
    explicit character(token tok);

    type::ptr _infer(semantics::scope& s) override;
    llvm::Value* make_rvalue(irconstructer &irc, IRScope& irscope) override;

    bool is_primary() const noexcept override;
    bool is_l_value() const noexcept override;
};

}

}
