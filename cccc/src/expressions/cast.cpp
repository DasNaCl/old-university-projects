#include <expressions/cast.hpp>

namespace mhc4
{

namespace expressions
{

expression::ptr cast(token tok, type::ptr typ, expression::ptr operand)
{
    // not suppported, might add as feature in future
    ((void*)&tok),((void*)&typ),std::move(operand);

    throw cast_expressions_not_supported_exception();
}

}

}
