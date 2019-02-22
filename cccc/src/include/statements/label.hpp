#pragma once

#include <statements/statement.hpp>

#include <lex/token.hpp>

#include <vector>

namespace mhc4
{

class source_location;

namespace statements
{

class label : public statement
{
public:
    explicit label(const source_location& loc, token id, statement::ptr follow);

    void print_statement(std::size_t depth) const override;
    void print(std::size_t depth) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
private:
    token id;
    statement::ptr stmt;
};

}

}
