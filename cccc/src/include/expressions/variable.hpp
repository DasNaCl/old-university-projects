#pragma once

#include <ast/primary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class variable : public ast::primary_printer
{
public:
    explicit variable(token tok);

    token name() const;

    type::ptr _infer(semantics::scope& s) override;
    llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    type::ptr struct_definition(semantics::scope& s);

    bool is_primary() const noexcept override;

    bool is_l_value() const noexcept override;
};

}

}
