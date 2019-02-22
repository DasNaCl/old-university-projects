#include <expressions/string.hpp>

#include <ir/irconstructer.hpp>

#include <types/integral.hpp>
#include <types/array.hpp>
#include <types/ptr.hpp>

#include <llvm/IR/Constants.h>

#include <algorithm>

namespace mhc4
{

namespace expressions
{

string::string(token tok)
    : primary_printer(tok)
{
    __expr_typ = util::make_unique<types::ptr>(util::make_unique<types::_integral>(tok::keyword_char));
}

type::ptr string::_infer(semantics::scope& )
{
    return util::make_unique<types::ptr>(util::make_unique<types::_integral>(tok::keyword_char));
    //return util::make_unique<types::array>(util::make_unique<types::_integral>(tok::keyword_char), tok.userdata().size() - 2);
}

llvm::Value* string::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    return irc._builder.CreateGlobalStringPtr(make_val().c_str());
}

std::size_t string::length() const
{
    // add one for null termination
    return make_val().size() + 1;
}

bool string::is_primary() const noexcept
{
    return true;
}

bool string::is_l_value() const noexcept
{
    return false;
}

std::string string::make_val() const noexcept
{
    std::string str = tok.userdata();

    constexpr static const char escapes[] = "\\?\"\'abfnrtv";
    constexpr static const char escaped[] = "\\\?\"\'\a\b\f\n\r\t\v";

    std::string str2;
    for(std::size_t i = 1; i < str.size()-1; ++i)
    {
        if(str[i] == '\\')
        {
            //eat the escape
            ++i;

            auto it = std::find(std::begin(escapes), std::end(escapes), str[i]);
            assert(it != std::end(escapes));
            str2.push_back(escaped[it - std::begin(escapes)]);
        }
        else
            str2.push_back(str[i]);
    }
    return str2;
}

}

}
