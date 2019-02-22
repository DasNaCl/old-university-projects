#include <parse/parser.hpp>

#include <lex/lexer.hpp>

#include <statements/declaration.hpp>
#include <statements/expression.hpp>
#include <statements/continue.hpp>
#include <statements/function.hpp>
#include <statements/return.hpp>
#include <statements/block.hpp>
#include <statements/break.hpp>
#include <statements/empty.hpp>
#include <statements/label.hpp>
#include <statements/while.hpp>
#include <statements/goto.hpp>
#include <statements/if.hpp>

#include <expressions/function_call.hpp>
#include <expressions/member_access.hpp>
#include <expressions/array_access.hpp>
#include <expressions/condition.hpp>
#include <expressions/variable.hpp>
#include <expressions/primary.hpp>
#include <expressions/binary.hpp>
#include <expressions/unary.hpp>
#include <expressions/cast.hpp>

#include <types/function.hpp>
#include <types/integral.hpp>
#include <types/struct.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <cassert>
#include <sstream>
#include <vector>
#include <limits>

namespace mhc4
{

static token dummy(source_location("", -1, -1), tok::end_of_file);

parser::parser(lexer& lexer)
  : basic_parser(lexer),
    current_token(lexer.lex()),
    lookahead(current_token, current_token),
    statements()
{
    next_token();
    next_token();
}

void parser::parse()
{
    if(peek(tok::end_of_file))
        throw empty_parse_exception();

    while(!peek(tok::end_of_file))
    {
        if(types::is_type(current_token.get_kind()))
            statements.push_back(parse_external_declaration());
        else
            throw_error(current_token.get_location(), "Invalid external declaration");
    }
}

std::vector<statement::ptr>& parser::ast()
{
    return statements;
}

const std::vector<statement::ptr>& parser::ast() const
{
    return statements;
}

void parser::next_token()
{
    current_token = lookahead.first;
    lookahead.first = lookahead.second;
    lookahead.second = lex.lex();
}

bool parser::accept(tok::token_kind tok)
{
    if(current_token.is_kind_of(tok))
    {
        next_token();
        return true;
    }
    return false;
}

void parser::expect(tok::token_kind tok)
{
    if(!accept(tok))
        throw_error(current_token.get_location(), "Expected other token");
}

void parser::expect_or(tok::token_kind tok0, tok::token_kind tok1)
{
    if(!accept(tok0) && !accept(tok1))
        throw_error(current_token.get_location(), "Expected other token");
}

bool parser::peek(tok::token_kind tok)
{
    return current_token.get_kind() == tok;
}

void parser::throw_error(const source_location& loc, const std::string& str)
{
    throw std::invalid_argument(loc.filepath() + ":"
                                + std::to_string(loc.line()) + ":"
                                + std::to_string(loc.column()) + ": error: "
                                + str);
}

statement::ptr parser::parse_external_declaration()
{
    return parse_declaration(true);
}

void parser::parse_direct_declarator_helper(parser::declarator& decl, bool infunc)
{
    // we ignore tokens that we don't have a case for by design
    switch(current_token.get_kind())
    {
    case tok::l_parent:
    {
        expect(tok::l_parent);

        std::vector<std::pair<std::size_t, token>> params;
        std::vector<type::ptr> param_types;
        std::size_t par_count = 0U;
        std::size_t names = 0U;
        while(!peek(tok::r_parent))
        {
            auto loc = current_token.get_location();
            auto stmt = parse_declaration(false, true);
            statements::declaration* decl = static_cast<statements::declaration*>(stmt.get());
            token tok = dummy;
            if(decl->has_name())
            {
                tok = decl->get_name();
                ++names;
            }

            if(decl->has_expr())
                throw_error(loc, "Compiler doesn't support initialized parameters");
            param_types.emplace_back(decl->rm_type());
            params.push_back(std::make_pair(par_count++, tok));

            //prepare for potential next parameter
            if(accept(tok::comma))
            {
                if(peek(tok::r_parent) || peek(tok::comma))
                {
                    throw_error(current_token.get_location(), "Too few parameters!");
                }
            }
        }
        if(params.size() == 1U && param_types.front()->is_void())
            params.clear();

        expect(tok::r_parent);
        if(!params.empty() && names)
        {
            decl.pars = params;
        }
        decl.typ = util::make_unique<types::function>(std::move(decl.typ), std::move(param_types));

        parse_direct_declarator_helper(decl, infunc);
    } break;
    case tok::lesscolon:
    case tok::l_square:
    {
        expect_or(tok::l_square, tok::lesscolon);
        // parse array declarator
        // TODO: possibly parse an assignment-expression here,
        //       then get it's constant value
        if(!peek(tok::r_square) && !peek(tok::colongreater))
            parse_expression();
// NOTE: Order might be twisted here as well

        decl.typ = util::make_unique<types::array>(std::move(decl.typ));
        expect_or(tok::r_square, tok::colongreater);
        parse_direct_declarator_helper(decl, infunc);
    } break;
    default:
        // ignore anything else
        break;
    }
}

void parser::parse_direct_declarator(parser::declarator& decl, bool infunc)
{
    if(infunc && current_token.is_kind_of(tok::r_parent))
    {
        return;
    }
    if(infunc && current_token.is_kind_of(tok::l_parent) && types::is_type(lookahead.first.get_kind()))
    {
        // here we are potentially something like
        // void (*(int))(char*)
        parse_direct_declarator_helper(decl, infunc);
        return;
    }

    switch(current_token.get_kind())
    {
    default:
        throw_error(current_token.get_location(), "Invalid direct declarator: \"" + current_token.userdata() + "\"");
        break;
    case tok::l_parent:
    {
        expect(tok::l_parent);

        parser::declarator thunkdecl {decl.id, util::make_unique<types::thunk>(), {}};
        parse_declarator(thunkdecl, infunc);

        expect(tok::r_parent);
        parse_direct_declarator_helper(decl, infunc);

        types::thunk* thunktyp = static_cast<types::thunk*>(thunkdecl.typ->get_any_unprepared_thunk());
        assert(thunktyp);
        thunktyp->prepare(std::move(decl.typ));
        decl.typ = std::move(thunkdecl.typ);

        if(decl.id == dummy && thunkdecl.id != dummy)
            decl.id = thunkdecl.id;
        if(decl.pars.empty() && !thunkdecl.pars.empty())
            decl.pars = thunkdecl.pars;

        return;
    }
    case tok::identifier:
    {
        decl.id = current_token;
        expect(tok::identifier);
        parse_direct_declarator_helper(decl, infunc);
        return;
    }
    }
}

void parser::parse_declarator(parser::declarator& decl, bool infunc)
{
    while(accept(tok::star))
        decl.typ = util::make_unique<types::ptr>(std::move(decl.typ));
    parse_direct_declarator(decl, infunc);
    return;
}

//external declaration is just parse_declaration or parse_function_definition
statement::ptr parser::parse_declaration(bool external, bool infunc)
{
    // 6.7 Declarations (1) type specifier
    auto loc = current_token.get_location();
    auto typ = parse_type();

    // we might end early if we are not an external declaration
    if(peek(tok::semicolon))
    {
        if(external && !typ->is_struct())
        {
            throw_error(current_token.get_location(), "External declaration must not omit a name");
        }
        expect(tok::semicolon);
        return util::make_unique<statements::declaration>(loc, std::move(typ));
    }
    if(infunc && (peek(tok::comma) || peek(tok::r_parent)))
    {
        return util::make_unique<statements::declaration>(loc, std::move(typ));
    }

    declarator decl{dummy, std::move(typ), {}};
    parse_declarator(decl, infunc);
    if(decl.typ->is_void() && decl.id != dummy)
    {
        throw_error(decl.id.get_location(), "variable or field declared void");
    }

    if(accept(tok::semicolon) || (infunc && (peek(tok::r_parent) || peek(tok::comma))))
    {
        return util::make_unique<statements::declaration>(loc, std::move(decl.typ), decl.id);
    }
    if(accept(tok::equal))
    {
        auto toret = util::make_unique<statements::declaration>(loc, std::move(decl.typ), decl.id, parse_expression());
        expect(tok::semicolon);
        return toret;
    }
    if(!external)
    {
        throw_error(current_token.get_location(), "Broken declaration");
    }

    auto fun = (dynamic_cast<types::function*>(decl.typ.get()));
    assert(fun);
    for(auto& d : decl.pars)
    {
        if(d.second == dummy)
        {
            throw_error(fun->parameter_type(d.first)->get_location(), "function with unnamed parameter");
        }
    }

    // parse function
    auto body = parse_block_statement();
    return util::make_unique<statements::function>(loc, std::move(decl.typ),
                                                   decl.id, decl.pars,
                                                   std::move(body));
}

type::ptr parser::parse_struct()
{
    auto structloc = current_token.get_location();
    expect(tok::keyword_struct);
    token_opt id(util::nullopt);
    if(peek(tok::identifier))
        id = parse_identifier();

    if(!peek(tok::l_brace) && !peek(tok::lesspercent))
    {
        if(!id.has())
        {
            throw_error(structloc, "struct has no identifier or body with declarations");
        }
        return util::make_unique<types::_struct>(id);
    }
    expect_or(tok::l_brace, tok::lesspercent);

    bool noinsert = true;
    auto str = util::make_unique<types::_struct>(id);
    while(!peek(tok::r_brace) && !peek(tok::percentgreater))
    {
        // parse struct-declaration
        auto stmt = parse_declaration(false, false);
        statements::declaration& decl = *static_cast<statements::declaration*>(stmt.get());
        str->insert(decl.rm_type(),
                    (decl.has_name()
                     ?
                     token_opt(decl.get_name())
                     :
                     token_opt(util::nullopt)));
        noinsert = false;
    }
    if(noinsert)
    {
        auto loc = current_token.get_location();
        throw_error(loc, "structs need at least one member");
    }
    expect_or(tok::r_brace, tok::percentgreater);

    return std::move(str);
}

type::ptr parser::parse_type()
{
    auto loc = current_token.get_location();
    type::ptr typ;
    switch(current_token.get_kind())
    {
    case tok::keyword_char:
    case tok::keyword_int:
    case tok::keyword_void:
    {
        typ = util::make_unique<types::_integral>(current_token.get_kind());
        next_token();
    } break;
    case tok::keyword_struct:
    {
        typ = parse_struct();
    } break;
    default:
        throw_error(current_token.get_location(), "Cannot parse type");
    }

    typ->set_location(loc);
    assert(typ);
    while(accept(tok::star))
        typ = util::make_unique<types::ptr>(std::move(typ));
    return typ;
}

expression::ptr parser::parse_operand()
{
    token t = current_token;
    switch(t.get_kind())
    {
    case tok::amp:
    case tok::star:
    case tok::exclaim:
    case tok::minus:
    case tok::minusminus:
    case tok::plus:
    case tok::plusplus:
    case tok::tilde:
    {
        next_token();
        expression::ptr operand = parse_expression(tok::precedence::Unary);
        return expressions::unary(t, false, std::move(operand));
    }
    case tok::keyword_sizeof:
    {
        next_token();
        if(!types::is_type(lookahead.first.get_kind()))
        {
            expression::ptr operand = parse_expression(tok::precedence::Unary);
            return expressions::unary(t, false, std::move(operand));
        }
        else
        {
            expect(tok::l_parent);
            type::ptr type = parse_type();
            declarator decl{dummy, std::move(type), {}};
            // we may be abstract, thus infunc = true
            parse_declarator(decl, true);
            if(decl.id != dummy)
            {
                throw_error(decl.id.get_location(), "Named typename!");
            }
            expect(tok::r_parent);
            return expressions::unary(t, false, std::move(decl.typ));
        }
    }

    case tok::l_parent:
    {
        expect(tok::l_parent);
        if(types::is_type(current_token.get_kind()))
        {
            type::ptr typ = parse_type();
            expect(tok::r_parent);
            expression::ptr operand = parse_expression(tok::precedence::Cast);
            return expressions::cast(t, std::move(typ), std::move(operand));
        }
        else
        {
            expression::ptr expr = parse_expression();
            expect(tok::r_parent);
            return expr;
        }
    }

    case tok::identifier:
    case tok::numeric_constant:
    case tok::char_constant:
    case tok::string_literal:
        next_token();
        return expressions::primary(t);

    default:
        throw_error(current_token.get_location(), "expected an expression");
    }

    assert(false);
    return expression::ptr();
}

expression::ptr parser::parse_expression(tok::precedence prec)
{
    expression::ptr exp = parse_operand();
    for(;;)
    {
        token t = current_token;
        auto kind = t.get_kind();
        if(tok::lprec(kind) < prec)
            return exp;
        next_token();

        switch(kind)
        {
        case tok::lesscolon: //<- digraph
        case tok::l_square: exp = parse_array_access(std::move(exp), t);  break;
        case tok::l_parent: exp = parse_function_call(std::move(exp), t); break;
        case tok::question: exp = parse_condition(std::move(exp), t);     break;
        case tok::period:
        case tok::arrow:    exp = parse_member_access(std::move(exp), t); break;

        default:
        {
            tok::precedence rprec = tok::rprec(kind);
            if(rprec == tok::precedence::None)
                exp = expressions::unary(t, true, std::move(exp));
            else
            {
                expression::ptr right = parse_expression(rprec);
                try
                {
                    exp = expressions::binary(t, std::move(exp), std::move(right));
                }
                catch(unknown_binary_op_exception& ex)
                {
                    throw_error(t.get_location(), ex.what());
                }
            }
        } break;
        }
    }
    assert(false);
    return exp;
}

expression::ptr parser::parse_array_access(expression::ptr expr, token t)
{
    expression::ptr at = parse_expression();
    expect_or(tok::r_square, tok::colongreater);
    return util::make_unique<expressions::array_access>(t, std::move(expr), std::move(at));
}

expression::ptr parser::parse_condition(expression::ptr expr, token t)
{
    expression::ptr consequence = parse_expression();
    expect(tok::colon);
    expression::ptr alternative = parse_expression(tok::precedence::Condition);
    return util::make_unique<expressions::condition>(t, std::move(expr),
                                                     std::move(consequence), std::move(alternative));
}

expression::ptr parser::parse_function_call(expression::ptr expr, token t)
{
    std::vector<expression::ptr> args;
    if(!peek(tok::r_parent))
    {
        do
        {
            args.push_back(parse_expression(tok::precedence::Assign));
        } while(accept(tok::comma));
    }
    expect(tok::r_parent);
    return util::make_unique<expressions::function_call>(t, std::move(expr), std::move(args));
}

expression::ptr parser::parse_member_access(expression::ptr from, token t)
{
    expression::ptr to = expressions::primary(parse_identifier());
    return util::make_unique<expressions::member_access>(t, std::move(from), std::move(to));
}

token parser::parse_identifier()
{
    token cur = current_token;
    if(peek(tok::identifier))
    {
        next_token();
        return cur;
    }
    else
        throw_error(current_token.get_location(), "no identifier present to parse");
    assert(false);
    return *((token*)(nullptr));
}

statement::ptr parser::parse_block_statement()
{
    auto loc = current_token.get_location();
    expect_or(tok::l_brace, tok::lesspercent);

    std::vector<statement::ptr> statements;
    for(;;)
    {
        switch(current_token.get_kind())
        {
            // 6.8.1 labeled statements
        case tok::identifier:

            // 6.8.4 selection statement
        case tok::keyword_if:

            // 6.8.5 iteration statement
        case tok::keyword_while:
        case tok::keyword_do:

            // 6.8.6 jump statements
        case tok::keyword_goto:
        case tok::keyword_continue:
        case tok::keyword_break:
        case tok::keyword_return:

            // 6.8.2 compound statement
        case tok::l_brace:
        case tok::lesspercent: // <- digraph

            // 6.8.3 expression and null statements
        case tok::semicolon:
            /// 6.5.1 primary expressions
        case tok::numeric_constant:
        case tok::char_constant:
        case tok::string_literal:
        case tok::l_parent:
            /// 6.5.3 unary operators
        case tok::amp:
        case tok::star:
        case tok::minus:
        case tok::exclaim:
        case tok::keyword_sizeof:
            /// 6.7.2 type specifiers
        case tok::keyword_void:
        case tok::keyword_char:
        case tok::keyword_int:
        case tok::keyword_struct:
        {
            statements.emplace_back(parse_statement(statement_type::outer));
        } continue;

        // end of block
        case tok::percentgreater: //<- digraph
        case tok::r_brace:
        {
            expect_or(tok::r_brace, tok::percentgreater);
        } break;
        // block not closed
        case tok::end_of_file:
        {
            throw_error(current_token.get_location(), "block not closed");
            goto next;
        }
        // anything else is invalid
        default:
        {
            next:
            throw_error(current_token.get_location(), "expect other token");
        }
        }
        break;
    }
    return util::make_unique<statements::block>(loc, std::move(statements));
}

statement::ptr parser::parse_if_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_if);
    expect(tok::l_parent);
    expression::ptr condition = parse_expression();
    expect(tok::r_parent);

    statement::ptr consequence = parse_statement(statement_type::inner);
    if(!peek(tok::keyword_else))
        return util::make_unique<statements::_if>(loc, std::move(condition), std::move(consequence));

    expect(tok::keyword_else);
    statement::ptr alternative = parse_statement(statement_type::inner);
    return util::make_unique<statements::_if>(loc, std::move(condition),
                                              std::move(consequence), std::move(alternative));
}

statement::ptr parser::parse_while_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_while);
    expect(tok::l_parent);
    expression::ptr condition = parse_expression();
    expect(tok::r_parent);
    statement::ptr body = parse_statement(statement_type::inner);

    return util::make_unique<statements::_while>(loc, std::move(condition), std::move(body));
}

statement::ptr parser::parse_do_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_do);
    statement::ptr body = parse_statement(statement_type::inner);
    expect(tok::keyword_while);
    expect(tok::l_parent);
    expression::ptr condition = parse_expression();
    expect(tok::r_parent);

    return util::make_unique<statements::_while>(loc, std::move(condition), std::move(body),
                                                 statements::_while::dowhile);
}

statement::ptr parser::parse_goto_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_goto);
    token id = parse_identifier();
    expect(tok::semicolon);

    return util::make_unique<statements::_goto>(loc, id);
}

statement::ptr parser::parse_continue_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_continue);
    expect(tok::semicolon);
    return util::make_unique<statements::_continue>(loc);
}

statement::ptr parser::parse_break_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_break);
    expect(tok::semicolon);
    return util::make_unique<statements::_break>(loc);
}

statement::ptr parser::parse_return_statement()
{
    auto loc = current_token.get_location();
    expect(tok::keyword_return);
    if(peek(tok::semicolon))
    {
        expect(tok::semicolon);
        return util::make_unique<statements::_return>(loc);
    }
    expression::ptr expr = parse_expression();
    expect(tok::semicolon);

    return util::make_unique<statements::_return>(loc, std::move(expr));
}

statement::ptr parser::parse_empty_statement()
{
    auto loc = current_token.get_location();
    expect(tok::semicolon);
    return util::make_unique<statements::empty>(loc);
}

statement::ptr parser::parse_expression_statement()
{
    auto loc = current_token.get_location();
    expression::ptr expr = parse_expression();
    expect(tok::semicolon);
    return util::make_unique<statements::_expression>(loc, std::move(expr));
}

statement::ptr parser::parse_labeled_statement()
{
    if(lookahead.first.get_kind() == tok::colon)
    {
        token name = parse_identifier();
        auto loc = current_token.get_location();
        expect(tok::colon);

        statement::ptr stmt = parse_statement(statement_type::inner);
        return util::make_unique<statements::label>(loc, name, std::move(stmt));
    }
    else
    {
        return parse_expression_statement();
    }
}

statement::ptr parser::parse_statement(parser::statement_type type)
{
    switch(current_token.get_kind())
    {
    // 6.8.1 labeled statement
    case tok::identifier:       return parse_labeled_statement();
    // 6.8.2 compund statement
    case tok::lesspercent: //<- digraph
    case tok::l_brace:          return parse_block_statement();
    // 6.8.4 selection statements
    case tok::keyword_if:       return parse_if_statement();
    // 6.8.5 iteration statements
    case tok::keyword_while:    return parse_while_statement();
    case tok::keyword_do:       return parse_do_statement();
    // 6.8.6 jump statements
    case tok::keyword_goto:     return parse_goto_statement();
    case tok::keyword_continue: return parse_continue_statement();
    case tok::keyword_break:    return parse_break_statement();
    case tok::keyword_return:   return parse_return_statement();
    // 6.9.1 function definitions
    case tok::keyword_void:
    case tok::keyword_char:
    case tok::keyword_int:
    case tok::keyword_struct:
    {
        if(type == statement_type::inner)
            throw_error(current_token.get_location(), "Declaration must not be in a inner statement");
        return parse_declaration();
    }
    // 6.8.3 expression and null statements
    case tok::semicolon:        return parse_empty_statement();
    ///6.5.1 primary expressions
    case tok::l_parent:   ///<- also: 6.7.6 declarators
    case tok::numeric_constant:
    case tok::char_constant:
    case tok::string_literal:
    ///6.5.3 unary operators
    case tok::keyword_sizeof:
    case tok::amp:
    case tok::star:       ///<- also: 6.7.6 declarators
    case tok::minus:
    case tok::exclaim:
        return parse_expression_statement();
    default:
        throw_error(current_token.get_location(), "expect other token");
    }
    assert(false);
    return statement::ptr();
}

}
