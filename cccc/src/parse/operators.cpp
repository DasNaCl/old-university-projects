#include <parse/operators.hpp>

namespace mhc4
{

namespace tok
{

precedence lprec(token_kind kind)
{
    switch(kind)
    {
    case tok::l_parent:
    case tok::lesscolon: // <- digraph
    case tok::l_square:
    case tok::minusminus:
    case tok::plusplus:
    case tok::period:
    case tok::arrow:
        return precedence::Postfix;

    case tok::percent:
    case tok::slash:
    case tok::star:
        return precedence::Multiply;

    case tok::minus:
    case tok::plus:
        return precedence::Add;

    case tok::lessless:
    case tok::greatergreater:
        return precedence::Shift;

    case tok::less:
    case tok::greater:
    case tok::lessequal:
    case tok::greaterequal:
        return precedence::Relational;

    case tok::equalequal:
    case tok::exclaimequal:
        return precedence::Equality;

    case tok::amp:
        return precedence::And;
    case tok::caret:
        return precedence::Xor;
    case tok::pipe:
        return precedence::Or;

    case tok::ampamp:
        return precedence::Logical_and;
    case tok::pipepipe:
        return precedence::Logical_or;

    case tok::question:
        return precedence::Condition;

    case tok::equal:
        return precedence::Assign;

    case tok::comma:
        return precedence::Expr;

    default:
        return precedence::None;
    }
}

precedence rprec(token_kind kind)
{
    switch(kind)
    {
    case tok::percent:
    case tok::slash:
    case tok::star:
        return precedence::Cast;

    case tok::minus:
    case tok::plus:
        return precedence::Multiply;

    case tok::lessless:
    case tok::greatergreater:
        return precedence::Add;

    case tok::less:
    case tok::greater:
    case tok::lessequal:
    case tok::greaterequal:
        return precedence::Shift;

    case tok::equalequal:
    case tok::exclaimequal:
        return precedence::Relational;

    case tok::amp:
        return precedence::Equality;

    case tok::caret:
        return precedence::And;
    case tok::pipe:
        return precedence::Xor;
    case tok::ampamp:
        return precedence::Or;
    case tok::pipepipe:
        return precedence::Logical_and;

    case tok::question:
        return precedence::Condition;
    case tok::equal:
    case tok::comma:
        return precedence::Assign;

    default:
        return precedence::None;
    }
}

}

}
