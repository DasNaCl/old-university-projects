#include <expressions/number.hpp>

#include <types/integral.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>

#include <sstream>

namespace mhc4
{

namespace expressions
{

number::number(token tok)
    : primary_printer(tok)
{
    __expr_typ = util::make_unique<types::_integral>(tok::keyword_int);
}

type::ptr number::_infer(semantics::scope& )
{
    return util::make_unique<types::_integral>(tok::keyword_int);
}

llvm::Value* number::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    long long num = 0LL;
    std::stringstream ss(tok.userdata());
    if(!(ss >> num))
    {
        ; // very soft error, should emit a warning here or something
    }
    return irc._builder.getInt32(num);
}

bool number::is_primary() const noexcept
{
    return true;
}

bool number::is_null() const noexcept
{
    return tok.userdata() == "0";
}

bool number::is_l_value() const noexcept
{
    return false;
}

}

}
