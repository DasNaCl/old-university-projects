#include <expressions/minus_unary.hpp>

namespace mhc4
{

namespace expressions
{

minus::minus(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
