#include <expressions/primary.hpp>

#include <expressions/character.hpp>
#include <expressions/variable.hpp>
#include <expressions/string.hpp>
#include <expressions/number.hpp>

#include <cassert>
#include <util>

namespace mhc4
{

namespace expressions
{

expression::ptr primary(token tok)
{
    switch(tok.get_kind())
    {
    case tok::identifier:       return util::make_unique<expressions::variable>(tok);
    case tok::numeric_constant: return util::make_unique<expressions::number>(tok);
    case tok::string_literal:   return util::make_unique<expressions::string>(tok);
    case tok::char_constant:    return util::make_unique<expressions::character>(tok);
    default: assert(false);
    }
    return expression::ptr();
}

}

}
