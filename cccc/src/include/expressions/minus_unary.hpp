#pragma once

#include <ast/unary_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

class minus : public ast::unary_printer
{
public:
    explicit minus(token op, bool postfix, expression::ptr operand);
};

}

}
