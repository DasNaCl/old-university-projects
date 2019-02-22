#pragma once

#include <expressions/expression.hpp>

#include <ast/statement_semicolon_printer.hpp>

#include <lex/token.hpp>

#include <types/type.hpp>

#include <ir/IRScope.hpp>

#include <vector>
#include <memory>

namespace mhc4
{

class source_location;

namespace statements
{

class declaration : public ast::statement_semicolon_printer
{
public:
    explicit declaration(const source_location& loc,
                         type::ptr t);
    explicit declaration(const source_location& loc,
                         type::ptr t, token name);
    explicit declaration(const source_location& loc, type::ptr t,
                         token name, expression::ptr expr);

    void print(std::size_t) const override;

    void infer(semantics::scope& s) const override;
    void generate_ir(irconstructer& irc, IRScope& irscope) override;

    bool has_name() const noexcept;
    bool has_expr() const noexcept;

    type::ptr rm_type();
    const token& get_name() const noexcept;
private:
    void generate_ir_for_func_type(irconstructer& irc, IRScope& irscope);
    void generate_ir_for_nonfunc_type(irconstructer& irc, IRScope& irscope);
private:
    type::ptr t;
    token_opt name;
    expression_opt expr;
};

}

}
