#pragma once

#include <expressions/expression.hpp>

#include <statements/statement.hpp>

#include <ir/IRScope.hpp>

#include <vector>
#include <util>

namespace mhc4
{

class source_location;

namespace statements
{

class _if : public statement
{
public:
    explicit _if(const source_location& loc,
                 expression::ptr condition,
                 statement::ptr consequence);
    explicit _if(const source_location& loc,
                 expression::ptr condition,
                 statement::ptr consequence,
                 statement::ptr alternative);

    bool is_if() const noexcept override;
    void do_not_indent_first() noexcept;

    void print_statement(std::size_t d) const override;
    void print(std::size_t d) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
private:
    expression::ptr condition;
    statement::ptr consequence;
    statement_opt alternative;

    bool indent_first;
};

}

}
