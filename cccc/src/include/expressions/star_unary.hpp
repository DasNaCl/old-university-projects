#pragma once

#include <ast/unary_printer.hpp>

namespace mhc4
{

namespace expressions
{

class star : public ast::unary_printer
{
public:
    explicit star(token op, bool postfix, expression::ptr operand);
};

}

}
