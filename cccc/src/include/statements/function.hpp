#pragma once

#include <expressions/expression.hpp>

#include <statements/statement.hpp>

#include <types/type.hpp>

#include <lex/token.hpp>

#include <vector>

namespace mhc4
{

class source_location;

namespace statements
{

class function : public statement
{
public:
    explicit function(const source_location& loc, type::ptr functyp, token name,
                      const std::vector<std::pair<std::size_t, token>>& params,
                      statement::ptr body);
    void print(std::size_t d) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
private:
    type::ptr functyp;
    token name;
    std::vector<std::pair<std::size_t, token>> parameter_names;
    statement::ptr body;
};

}

}
