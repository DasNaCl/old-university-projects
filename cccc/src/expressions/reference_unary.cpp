#include <expressions/reference_unary.hpp>

namespace mhc4
{

namespace expressions
{

reference::reference(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
