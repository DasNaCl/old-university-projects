#pragma once

#include <ast/unary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class increment : public ast::unary_printer
{
public:
    explicit increment(token op, bool postfix, expression::ptr operand);
};

}

}
