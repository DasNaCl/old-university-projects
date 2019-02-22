#pragma once

#include <ast/statement_semicolon_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

class source_location;

namespace statements
{

class empty : public ast::statement_semicolon_printer
{
public:
    explicit empty(const source_location& loc);

    void print(std::size_t) const override;

    void infer(semantics::scope&) const override;

    void generate_ir(irconstructer&, IRScope&) override;
};

}

}
