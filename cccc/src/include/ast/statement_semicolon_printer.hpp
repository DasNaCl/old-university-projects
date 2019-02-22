#pragma once

#include <statements/statement.hpp>

namespace mhc4
{

namespace ast
{

class statement_semicolon_printer : public statement
{
protected:
    explicit statement_semicolon_printer(const source_location& loc);
public:
    void print_statement(std::size_t depth) const override;
};

}

}
