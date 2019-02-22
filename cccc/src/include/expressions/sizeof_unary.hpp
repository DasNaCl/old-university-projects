#pragma once

#include <expressions/expression.hpp>
#include <types/type.hpp>
#include <lex/token.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

// note: this is not derived from unary_printer by design!
class sizeof_ : public expression
{
public:
    explicit sizeof_(token op, bool, expression::ptr operand);
    explicit sizeof_(token op, bool, type::ptr typ);

    type::ptr _infer(semantics::scope& s) override;

    llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) override;

    void print() const override;
    bool is_l_value() const noexcept override;
private:
    token op;
    expression_opt operand;
    type_opt typ;
};

}

}
