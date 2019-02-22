#pragma once

#include <ast/statement_semicolon_printer.hpp>

#include <vector>

namespace mhc4
{

class source_location;

namespace statements
{

class _break : public ast::statement_semicolon_printer
{
public:
    explicit _break(const source_location& loc);

    void print(std::size_t) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
};

}

}
