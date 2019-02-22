#include <expressions/binary.hpp>


#include <expressions/arithmetic_binary.hpp>
#include <expressions/comparison_binary.hpp>
#include <expressions/inequality_binary.hpp>
#include <expressions/logical_and.hpp>
#include <expressions/logical_or.hpp>
#include <expressions/assign.hpp>

#include <util>

namespace mhc4
{

namespace expressions
{

expression::ptr binary(token tok, expression::ptr left, expression::ptr right)
{
    switch(tok.get_kind())
    {
    case tok::star:
    case tok::slash:
    case tok::plus:
    case tok::minus:        return util::make_unique<arithmetic>(tok, std::move(left), std::move(right));

    case tok::ampamp:       return util::make_unique<logical_and>(tok, std::move(left), std::move(right));
    case tok::pipepipe:     return util::make_unique<logical_or>(tok, std::move(left), std::move(right));

    case tok::equalequal:
    case tok::exclaimequal: return util::make_unique<comparison>(tok, std::move(left), std::move(right));

    case tok::less:
    case tok::lessequal:
    case tok::greater:
    case tok::greaterequal: return util::make_unique<inequality>(tok, std::move(left), std::move(right));

    case tok::equal:        return util::make_unique<assign>(tok, std::move(left), std::move(right));

    default: throw unknown_binary_op_exception(tok);
    }
}

}

}
