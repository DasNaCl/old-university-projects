#pragma once

#include <lex/token.hpp>

#include <ostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

namespace mhc4
{

class lexer
{
private:
    static constexpr const std::size_t token_kind_pos  = 0U;
    static constexpr const std::size_t token_begin_pos = 1U;
    static constexpr const std::size_t token_end_pos   = 2U;
    static constexpr const std::size_t token_row_pos   = 3U;
    static constexpr const std::size_t token_col_pos   = 4U;

    static constexpr const std::size_t token_total_positions = 5U;
public:
    explicit lexer(const std::string& filepath, const char* file_content, std::size_t file_length);

    void print_all();

    token lex();
private:
    void lex_all();
private:
    void throw_error(std::size_t row, std::size_t column,
                     std::string tokstr) const;
private:
    unsigned char cur_char(int offset = 0) const
    {
        if(total + offset >= file_length)
            throw_error(row, column,
                        {content + old_total, content + total});
        return static_cast<unsigned char>(content[total + offset]);
    }
    std::tuple<bool, std::uintptr_t, std::size_t, std::size_t> find_comment();
private:
    const std::string& filepath;
    const char* content;
    std::size_t file_length;

    std::size_t row;
    std::size_t column;
    std::size_t total;

    std::size_t old_total;

    std::vector<std::uintptr_t> tokarr;
    std::size_t curtok;
    std::size_t tokarrsize;

    bool print;
};



}
