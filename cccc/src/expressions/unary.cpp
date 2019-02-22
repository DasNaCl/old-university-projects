#include <expressions/unary.hpp>

#include <expressions/decrement_unary.hpp>
#include <expressions/increment_unary.hpp>
#include <expressions/reference_unary.hpp>
#include <expressions/sizeof_unary.hpp>
#include <expressions/minus_unary.hpp>
#include <expressions/plus_unary.hpp>
#include <expressions/star_unary.hpp>
#include <expressions/negation.hpp>

#include <cassert>

namespace mhc4
{

namespace expressions
{

expression::ptr unary(token tok, bool postfix, expression::ptr operand)
{
    switch(tok.get_kind())
    {
    case tok::exclaim:        return util::make_unique<expressions::negation>(tok, postfix, std::move(operand));
    case tok::minus:          return util::make_unique<expressions::minus>(tok, postfix, std::move(operand));
    case tok::plus:           return util::make_unique<expressions::plus>(tok, postfix, std::move(operand));
    case tok::star:           return util::make_unique<expressions::star>(tok, postfix, std::move(operand));
    case tok::amp:            return util::make_unique<expressions::reference>(tok, postfix, std::move(operand));
    case tok::keyword_sizeof: return util::make_unique<expressions::sizeof_>(tok, postfix, std::move(operand));
    case tok::plusplus:       return util::make_unique<expressions::increment>(tok, postfix, std::move(operand));
    case tok::minusminus:     return util::make_unique<expressions::decrement>(tok, postfix, std::move(operand));
    default: assert(false);
    }
    return expression::ptr();
}

expression::ptr unary(token tok, bool postfix, type::ptr type)
{
    switch(tok.get_kind())
    {
    case tok::keyword_sizeof: return util::make_unique<expressions::sizeof_>(tok, postfix, std::move(type));
    default: assert(false);
    }
    return expression::ptr();
}

}

}
