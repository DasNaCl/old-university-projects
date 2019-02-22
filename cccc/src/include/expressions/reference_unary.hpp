#pragma once

#include <ast/unary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class reference : public ast::unary_printer
{
public:
    explicit reference(token op, bool postfix, expression::ptr operand);
};

}

}
