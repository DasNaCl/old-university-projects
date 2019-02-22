#include <expressions/decrement_unary.hpp>

namespace mhc4
{

namespace expressions
{

decrement::decrement(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
