#pragma once

#include <expressions/expression.hpp>
#include <statements/statement.hpp>

#include <ir/IRScope.hpp>

#include <vector>

namespace mhc4
{

class source_location;

namespace statements
{

class _while : public statement
{
public:
    enum kind
    {
        dowhile,
        normal
    };
public:
    explicit _while(const source_location& loc,
                    expression::ptr condition, statement::ptr body,
                    kind t = normal);
    void print(std::size_t d) const override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;
private:
    expression::ptr condition;
    statement::ptr body;
    kind typ;
};

}

}
