#include <expressions/plus_unary.hpp>

namespace mhc4
{

namespace expressions
{

plus::plus(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
