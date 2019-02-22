#pragma once

#include <lex/token_kind.hpp>

namespace mhc4
{

namespace tok
{

enum class precedence
{
     None,
     Expr,
     Assign,
     Condition,
     Logical_or,
     Logical_and,
     Or,
     Xor,
     And,
     Equality,
     Relational,
     Shift,
     Add,
     Multiply,
     Cast,
     Unary,
     Postfix,
     Primary
};

precedence lprec(token_kind kind);
precedence rprec(token_kind kind);

}

}
