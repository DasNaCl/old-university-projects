#pragma once

#include <lex/source_location.hpp>

#include <ir/IRScope.hpp>

#include <memory>
#include <util>

namespace mhc4
{

class irconstructer;

namespace semantics
{
    class scope;
}

class statement
{
public:
    using ptr = std::unique_ptr<statement>;
protected:
    explicit statement(const source_location& loc);
public:
    virtual ~statement() noexcept;

    virtual bool is_if() const noexcept;
    virtual bool is_block() const noexcept;

    virtual void infer(semantics::scope& s) const = 0;

    virtual void generate_ir(irconstructer& irc, IRScope& irscope) = 0;

    virtual void print_statement(std::size_t depth) const;
protected:
    virtual void print(std::size_t depth) const = 0;
protected:
    source_location loc;
};

using statement_opt = util::optional<statement::ptr>;

}
