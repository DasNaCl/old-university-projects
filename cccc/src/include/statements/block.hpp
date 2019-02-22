#pragma once

#include <statements/statement.hpp>

#include <vector>

namespace mhc4
{

namespace statements
{

class block : public statement
{
public:
    explicit block(const source_location& loc,
                   std::vector<statement::ptr> statements);

    void do_not_indent_first() noexcept;
    void do_not_add_newline() noexcept;

    bool is_block() const noexcept override;

    void infer(semantics::scope& s) const override;

    void generate_ir(irconstructer& irc, IRScope& irscope) override;

    void generate_ir_after_block(irconstructer& irc, IRScope& irscope);

    void infer_in_scope(semantics::scope& s) const;

    void print_statement(std::size_t depth) const override;
    void print(std::size_t depth) const override;
private:
    std::vector<statement::ptr> statements;

    bool indent_first;
    bool newline;
};

}

}
