#include <expressions/character.hpp>

#include <ir/irconstructer.hpp>

#include <types/integral.hpp>

namespace mhc4
{

namespace expressions
{

character::character(token tok)
    : primary_printer(tok)
{
    __expr_typ = util::make_unique<types::_integral>(tok::keyword_char);
}

type::ptr character::_infer(semantics::scope& )
{
    return util::make_unique<types::_integral>(tok::keyword_int);
}

llvm::Value* character::make_rvalue(irconstructer &irc, IRScope& irscope)
{
    // note: userdata contains the single quotation marks
    assert(tok.userdata().size() == 3);
    return irc._builder.getInt8(tok.userdata()[1]);
}

bool character::is_primary() const noexcept
{
    return true;
}

bool character::is_l_value() const noexcept
{
    return false;
}

}

}
