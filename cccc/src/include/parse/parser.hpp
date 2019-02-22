#pragma once

#include <basic_parser.hpp>

#include <lex/lexer.hpp>
#include <lex/token.hpp>

#include <parse/operators.hpp>

#include <expressions/expression.hpp>
#include <statements/statement.hpp>
#include <types/type.hpp>

#include <stdexcept>
#include <array>

namespace mhc4
{

class empty_parse_exception : public std::invalid_argument
{
public:
    empty_parse_exception()
        : invalid_argument("File is empty, nothing to parse")
    {  }
};

class parser : public basic_parser
{
public:
    enum class statement_type
    {
        outer,
        inner,
    };
public:
    explicit parser(lexer& lexer);
    ~parser() override {}

    void parse() override;

    std::vector<statement::ptr>& ast();
    const std::vector<statement::ptr>& ast() const;
private:
    void next_token();
    bool accept(tok::token_kind tok);
    void expect(tok::token_kind tok);
    bool peek(tok::token_kind tok);

    void expect_or(tok::token_kind tok0, tok::token_kind tok1);

    void throw_error(const source_location& loc, const std::string& str);

    statement::ptr parse_external_declaration();

    token parse_identifier();
    type::ptr parse_struct();
    type::ptr parse_type();

    expression::ptr parse_expression(tok::precedence prec = tok::precedence::Expr);
    expression::ptr parse_operand();
    expression::ptr parse_array_access(expression::ptr exp, token tok);
    expression::ptr parse_function_call(expression::ptr exp, token tok);
    expression::ptr parse_condition(expression::ptr exp, token tok);
    expression::ptr parse_member_access(expression::ptr exp, token tok);

    statement::ptr parse_block_statement();
    statement::ptr parse_if_statement();
    statement::ptr parse_while_statement();
    statement::ptr parse_do_statement();
    statement::ptr parse_goto_statement();
    statement::ptr parse_continue_statement();
    statement::ptr parse_break_statement();
    statement::ptr parse_return_statement();
    statement::ptr parse_empty_statement();
    statement::ptr parse_expression_statement();
    statement::ptr parse_labeled_statement();

public:
struct declarator
{
    token id;
    type::ptr typ;

    std::vector<std::pair<std::size_t, token>> pars;
};
private:

    statement::ptr parse_declaration(bool external = false, bool infunc = false);
    void parse_declarator(declarator& decl, bool infunc);
    void parse_direct_declarator(declarator& decl, bool infunc);
    void parse_direct_declarator_helper(declarator& decl, bool infunc);

    statement::ptr parse_statement(statement_type type);
private:
    token current_token;
    std::pair<token, token> lookahead;

    std::vector<statement::ptr> statements;
};

}
