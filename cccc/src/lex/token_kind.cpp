#include <lex/token_kind.hpp>

#include <unordered_map>
#include <algorithm>
#include <sstream>

#include <iostream>

namespace mhc4
{
namespace tok
{

    struct hash
    {
        std::size_t operator()(const char* str) const noexcept
        {
            std::size_t hash = 0U;
            for(int i = 0; i < 5 && (str + i); ++i)
                hash ^= *(str + i);
            return hash;
        }
    };

    const static std::unordered_map<std::string, token_kind> str_to_token_kind =
    {
#define KEYWORD(X) { #X, keyword_ ## X },
#define PUNCTUATOR(X, Y) { Y, X },
#include <lex/token_kind.def>
        { std::string("eof"), end_of_file },
        { std::string("<?>"), unknown }
    };

    const char* keyword_name(token_kind kind)
    {
        switch(kind)
        {
#define KEYWORD(X) case keyword_ ## X: return #X;
#include <lex/token_kind.def>
        default: break;
        }
        return nullptr;
    }

    const char* punctuator_name(token_kind kind)
    {
        switch(kind)
        {
#define PUNCTUATOR(X,Y) case X: return Y;
#include <lex/token_kind.def>
        default: break;
        }
        return nullptr;
    }

    bool is_identifier(const std::string& str)
    {
        // Maybe use Latin1 locale??
        return str.front() > '9' &&
            std::all_of(str.begin(), str.end(),
                        [](unsigned char i)
                        { return   i == '_'
                                || std::isalnum(i); });
    }

    token_kind from_string(const std::string& str)
    {
        const auto it = str_to_token_kind.find(str);

        if(it == str_to_token_kind.end())
        {
            // Can be an identifier, numeric_constant,
            // char_constant or string_literal

            if(str[0] == '\"')
                return string_literal;
            else if(str[0] == '\'')
                return char_constant;

            // try to parse as number
            std::stringstream ss(str);
            double buf;
            if(ss >> buf)
                return numeric_constant;
            else if(is_identifier(str))
                return identifier;
            return unknown;
        }
        return it->second;
    }

}
}
