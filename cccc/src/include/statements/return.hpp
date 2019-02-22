#pragma once

#include <expressions/expression.hpp>

#include <ast/statement_semicolon_printer.hpp>

#include <vector>

namespace mhc4
{

class source_location;

namespace statements
{

class _return : public ast::statement_semicolon_printer
{
public:
    explicit _return(const source_location& loc);
    explicit _return(const source_location& loc,
                     expression::ptr expr);

    void print(std::size_t) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
private:
    expression_opt expr;
};

}

}
