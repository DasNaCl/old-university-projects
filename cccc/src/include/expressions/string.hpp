#pragma once

#include <ast/primary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class string : public ast::primary_printer
{
public:
    explicit string(token tok);

    type::ptr _infer(semantics::scope& s) override;
    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    std::size_t length() const;
    bool is_primary() const noexcept override;
    bool is_l_value() const noexcept override;
private:
    std::string make_val() const noexcept;
};

}

}
