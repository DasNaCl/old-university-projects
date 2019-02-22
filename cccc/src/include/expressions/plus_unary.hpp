#pragma once

#include <ast/unary_printer.hpp>

#include <ir/IRScope.hpp>

namespace mhc4
{

namespace expressions
{

class plus : public ast::unary_printer
{
public:
    explicit plus(token op, bool postfix, expression::ptr operand);
};

}

}
