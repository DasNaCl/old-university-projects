#include <expressions/increment_unary.hpp>

namespace mhc4
{

namespace expressions
{

increment::increment(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
