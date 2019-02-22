#include <lex/token.hpp>
#include <lex/lexer.hpp>

#include <cassert>
#include <sstream>

namespace mhc4
{

token::token(source_location&& loc, tok::token_kind kind, const std::string& data)
    : loc(loc), kind(kind), data(data)
{  }

bool token::operator==(const token& tok) const noexcept
{
    return loc == tok.loc && kind == tok.kind && data == tok.data;
}

bool token::operator!=(const token& tok) const noexcept
{
    return !(loc == tok.loc && kind == tok.kind && data == tok.data);
}

tok::token_kind token::get_kind() const noexcept
{
    return kind;
}

void token::set_kind(tok::token_kind k) noexcept
{
    kind = k;
}

bool token::is_kind_of(tok::token_kind k) const noexcept
{
    return kind == k;
}

bool token::is_one_of(const std::vector<tok::token_kind>& data) const noexcept
{
    for(const auto& k : data)
        if(k != kind)
            return false;
    return true;
}

bool token::is_eof() const noexcept
{
    return !(is_kind_of(tok::end_of_file));
}

const source_location& token::get_location() const noexcept
{
    return loc;
}

std::string token::userdata() const
{
    return data;
}

std::string token::to_string() const
{
    std::stringstream ss;
    ss << loc << " ";

    using namespace tok;

    switch(kind)
    {
#define KEYWORD(x) case keyword_ ## x: { ss << (std::string("keyword ") + #x).c_str(); } break;
#define PUNCTUATOR(x, y) case x: { ss << (std::string("punctuator ") + y).c_str(); } break;
#include <lex/token_kind.def>

    case identifier: {
        ss << std::string("identifier ") + data; } break;
    case numeric_constant: {
        ss << std::string("constant ") + data; } break;
    case char_constant: {
        ss << std::string("constant ") + data; } break;
    case string_literal: {
        ss << std::string("string-literal ") + data; } break;

    default: // token 'unknown' or 'token_count'
        assert(false);
    }
    return ss.str();
}

}
