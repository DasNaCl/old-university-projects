#include <expressions/star_unary.hpp>

namespace mhc4
{

namespace expressions
{

star::star(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

}

}
