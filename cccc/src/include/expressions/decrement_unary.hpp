#pragma once

#include <ast/unary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class decrement : public ast::unary_printer
{
public:
    explicit decrement(token op, bool postfix, expression::ptr operand);
};

}

}
