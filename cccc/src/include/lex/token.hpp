#pragma once

#include <lex/source_location.hpp>
#include <lex/token_kind.hpp>

#include <vector>
#include <util>

namespace mhc4
{

class token
{
public:
    explicit token(source_location&& loc, tok::token_kind kind,
                   const std::string& data = "");

    bool operator==(const token& tok) const noexcept;
    bool operator!=(const token& tok) const noexcept;

    tok::token_kind get_kind() const noexcept;
    void set_kind(tok::token_kind k) noexcept;

    bool is_kind_of(tok::token_kind k) const noexcept;
    bool is_one_of(const std::vector<tok::token_kind>& data) const noexcept;

    bool is_eof() const noexcept;

    const source_location& get_location() const noexcept;

    std::string userdata() const;

    std::string to_string() const;

    friend std::hash<mhc4::token>;
private:
    source_location loc;
    tok::token_kind kind;
    std::string data;
};

using token_opt = util::optional<token>;

}

namespace std
{
    template <>
    struct hash<mhc4::token>
    {
        std::size_t operator()(const mhc4::token& k) const
        {
            return hash<std::string>()(k.loc.filepath())
                 ^ hash<std::size_t>()(k.loc.line())
                 ^ hash<std::size_t>()(k.loc.column())
                 ^ hash<char>()(k.kind)
                 ^ hash<std::string>()(k.data);
         }
    };
}
