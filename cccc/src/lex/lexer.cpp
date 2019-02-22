#include <lex/lexer.hpp>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <array>

namespace mhc4
{

namespace tok
{

const static std::unordered_map<std::string, token_kind> str_to_token_kind =
{
#define KEYWORD(X) { #X, keyword_ ## X },
#include <lex/token_kind.def>
    { std::string(""), unknown }
};

}

lexer::lexer(const std::string& filepath, const char* file_content,
             std::size_t file_length)
    : filepath(filepath),
      content(file_content),
      file_length(file_length),
      row(1U), column(1U), total(0U), tokarr(), curtok(0U), tokarrsize(0U),
      print(false)
{
    // add one to respect end_of_file
    tokarr.resize(token_total_positions * file_length);
}

token lexer::lex()
{
    if(curtok == 0U)
        lex_all();
    if(curtok >= tokarrsize)
        return token(source_location(filepath, row, column),
                     tok::end_of_file, "");
    const std::size_t offset = token_total_positions * curtok;
    ++curtok;

    const tok::token_kind kind = static_cast<tok::token_kind>(tokarr[offset + token_kind_pos]);
    const std::uintptr_t row   = tokarr[offset + token_row_pos];
    const std::uintptr_t col   = tokarr[offset + token_col_pos];

    const std::uintptr_t beg   = tokarr[offset + token_begin_pos];
    const std::uintptr_t end   = tokarr[offset + token_end_pos];

    const std::string tokstr(reinterpret_cast<const char*>(beg),
                             reinterpret_cast<const char*>(end));

    return token(source_location(filepath, row, col), kind, tokstr);
}

void lexer::print_all()
{
    print = true;
    lex_all();
}

void lexer::lex_all()
{
    for(; total < file_length; )
    {
        tok::token_kind kind = tok::end_of_file;
        std::string kind_str;

        char current_character = cur_char();

        old_total = total;
        std::size_t old_col = column;

        ++total; ++column;
        switch(current_character)
        {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
        {
            if(current_character == '\r')
            {
                if(total < file_length && cur_char() == '\n')
                {
                    ++total; // <- consume LF and treat CLRF
                }
            }
            if(current_character == '\r' || current_character == '\n')
            {
                column = 1U;
                ++row;
            }
        } continue;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            constexpr const char* numbers = "0123456789";
            const bool startsWithZero = current_character == '0';
            int counter = 0;
            while(total < file_length && std::count(numbers, numbers + 10, cur_char()))
            { ++column; ++total; ++counter; }
            if(counter > 1 && startsWithZero)
            {
                // we may not start with zero
                throw std::invalid_argument(filepath + ":" + std::to_string(row)
                                            + ":" + std::to_string(column) + ": error: "
                                            + "Numbers may not start with zero!");
            }
            kind = tok::numeric_constant;
            kind_str = "constant " + std::string(content + old_total, content + total);
        } break;

        case '<':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '<')
            {
                ++column;
                ++total;
                kind = tok::lessless;
                kind_str = "punctuator <<";
                if(total < file_length && cur_char() == '=')
                {
                    ++column;
                    ++total;
                    kind = tok::lesslessequal;
                    kind_str = "punctuator <<=";
                }
            }
            else
            {
                switch(v)
                {
                case '%':
                {
                    kind = tok::lesspercent;
                    kind_str = "punctuator <%";
                    ++column;
                    ++total;
                } break;
                case ':':
                {
                    kind = tok::lesscolon;
                    kind_str = "punctuator <:";
                    ++column;
                    ++total;
                } break;
                case '=':
                {
                    kind = tok::lessequal;
                    kind_str = "punctuator <=";
                    ++column;
                    ++total;
                } break;
                default:
                    kind = tok::less;
                    kind_str = "punctuator <";
                }
            }
        } break;

        // TODO: optimize all those operator o which can appear as o= as well
        case '^':
        {
            if(total < file_length && cur_char() == '=')
            {
                ++column;
                ++total;
                kind = tok::caretequal;
                kind_str = "punctuator ^=";
            }
            else
            {
                kind = tok::caret;
                kind_str = "punctuator ^";
            }
        } break;
        case '!':
        {
            if(total < file_length && cur_char() == '=')
            {
                ++column;
                ++total;
                kind = tok::exclaimequal;
                kind_str = "punctuator !=";
            }
            else
            {
                kind = tok::exclaim;
                kind_str = "punctuator !";
            }
        } break;
        case '*':
        {
            if(total < file_length && cur_char() == '=')
            {
                ++column;
                ++total;
                kind = tok::starequal;
                kind_str = "punctuator *=";
            }
            else
            {
                kind = tok::star;
                kind_str = "punctuator *";
            }
        } break;
        case '=':
        {
            if(total < file_length && cur_char() == '=')
            {
                ++column;
                ++total;
                kind = tok::equalequal;
                kind_str = "punctuator ==";
            }
            else
            {
                kind = tok::equal;
                kind_str = "punctuator =";
            }
        } break;

        case '#':
        {
            if(total < file_length && cur_char() == '#')
            {
                ++column;
                ++total;
                kind = tok::hashhash;
                kind_str = "punctuator ##";
            }
            else
            {
                kind = tok::hash;
                kind_str = "punctuator #";
            }
        } break;
        
        case '{':
        case '}':
        case '[':
        case ']':
        case '(':
        case ')':
        case ';':
        case ',':
        case '~':
        case '?':
        {
            kind = static_cast<tok::token_kind>(current_character);
            kind_str = "punctuator ";
            kind_str.push_back(current_character);
        } break;

        case '/':
        {
            if(total < file_length && cur_char() == '/')
            {
                // single line comment
                ++column;
                ++total;

                unsigned char c = total < file_length ? cur_char() : 0;
                bool doloop = total < file_length && c != '\n' && c != '\r' && ++total;
                while(doloop)
                {
                    ++column;
       
                    // if line ends in backslash, we continue to the next line
                    if(total < file_length && cur_char(-1) == '\\' && (cur_char() == '\n' || cur_char() == '\r'))
                    {
                        //consume newline
                        if(cur_char() == '\r')
                        {
                            ++total;
                            if(total < file_length && cur_char() == '\n')
                                ++total;
                        }
                        else
                            ++total;
                        column = 1U;
                        ++row;

                        doloop = true;
                    }
                    c = total < file_length && doloop ? cur_char() : 0;
                    doloop = total < file_length && c != '\n' && c != '\r' && ++total;
                }
                continue;
            }
            else if(total < file_length && cur_char() == '*')
            {
                ++column;
                ++total;
                auto res = find_comment();
                if(!std::get<0>(res))
                {
                    //unterminated comment
                    throw_error(std::get<2>(res), std::get<3>(res) -1,
                                {content + old_total, content + std::get<1>(res)});
                }
                else
                {
                    bool unused = false;
                    std::tie(unused, total, row, column) = res;
                    continue;
                }
            }
            else
            {
                if(total < file_length && cur_char() == '=')
                {
                    ++column;
                    ++total;
                    kind = tok::slashequal;
                    kind_str = "punctuator /=";
                }
                else
                {
                    kind = tok::slash;
                    kind_str = "punctuator /";
                }
            }              
        } break;

        case '\'':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '\\')
            {
                ++column;
                ++total;

                constexpr const char* escapes = "\\?\"\'abfnrtv";
                if(total < file_length && std::count(escapes, escapes + 11, cur_char()))
                {
                    ++column;
                    ++total;
                }
                else
                {
                    throw_error(row, column,
                                {content + old_total, content + total});
                }
            }
            else if(v != '\n' && v != '\r')
            {
                ++column;
                ++total;
            }
            if(total >= file_length || cur_char() != '\'')
            {
                // character constant doesn't terminate
                throw_error(row, column - 1,
                            {content + old_total, content + total});
            }
            // consume terminating smark
            ++column;
            ++total;
            kind = tok::char_constant;
            kind_str = "constant " + std::string(content + old_total, content + total);
        } break;


        case '-':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '>' || v == '-' || v == '=')
            {
                ++column;
                ++total;
                kind = (v == '>' ? tok::arrow
                        : (v == '-' ? tok::minusminus : tok::minusequal));
                kind_str = (v == '>' ? "punctuator ->"
                            : (v == '-' ? "punctuator --" : "punctuator -="));
            }
            else
            {
                kind = tok::minus;
                kind_str = "punctuator -";
            }
        } break;
        case '+':
        {
            const unsigned char v = total < file_length ? cur_char(): 0;
            if(v == '+' || v == '=')
            {
                ++column;
                ++total;
                kind = (v == '+' ? tok::plusplus : tok::plusequal);
                kind_str = (v == '+' ? "punctuator ++" : "punctuator +=");
            }
            else
            {
                kind = tok::plus;
                kind_str = "punctuator +";
            }
        } break;

        case '%':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '>')
            {
                ++column;
                ++total;
                kind = tok::percentgreater;
                kind_str = "punctuator %>";
            }
            else if(v == '=')
            {
                ++column;
                ++total;
                kind = tok::percentequal;
                kind_str = "punctuator %=";
            }
            else if(v == ':')
            {
                ++column;
                ++total;
                kind = tok::percentcolon;
                kind_str = "punctuator %:";

                if(total + 1 < file_length && cur_char() == '%' && cur_char(1) == ':')
                {
                    ++column; ++column;
                    ++total;  ++total;
                    kind = tok::percentcolonpercentcolon;
                    kind_str = "punctuator %:%:";
                }
            }
            else
            {
                kind = tok::percent;
                kind_str = "punctuator %";
            }
        } break;

        case '\"':
        {
            bool stop = false;
            do
            {
                if(total < file_length && cur_char() == '\\')
                {
                    ++column;
                    ++total;

                    constexpr const char* escapes = "\\?\"\'abfnrtv";
                    if(total < file_length && std::count(escapes, escapes + 11, cur_char()))
                    {
                        ++column;
                        ++total;
                    }
                    else
                        throw_error(row, column,
                                    {content + old_total, content + total});
                }
                else if(total < file_length && cur_char() == '\"')
                {
                    ++column;
                    ++total;
                    stop = true;
                }
                else if(total < file_length && cur_char() != '\n' && cur_char() != '\r')
                {
                    ++column;
                    ++total;
                }
                else
                    throw_error(row, column - 1,
                                {content + old_total, content + total});
            } while(!stop);

            kind = tok::string_literal;
            kind_str = "string-literal " + std::string(content + old_total, content + total);
        } break;

        case '&':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '&' || v == '=')
            {
                ++column;
                ++total;
                kind = (v == '&' ? tok::ampamp : tok::ampequal);
                kind_str = (v == '&' ? "punctuator &&" : "punctuator &=");
            }
            else
            {
                kind = tok::amp;
                kind_str = "punctuator &";
            }
        } break;
        case '|':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '|' || v == '=')
            {
                ++column;
                ++total;
                kind = (v == '|' ? tok::pipepipe : tok::pipeequal);
                kind_str = (v == '|' ? "punctuator ||" : "punctuator |=");
            }
            else
            {
                kind = tok::pipe;
                kind_str = "punctuator |";
            }
        } break;

        case '.':
        {
            if(total + 1 < file_length && cur_char() == '.' && cur_char(1) == '.')
            {
                ++column; ++column;
                ++total;  ++total;
                kind = tok::ellipsis;
                kind_str = "punctuator ...";
            }
            else
            {
                kind = tok::period;
                kind_str = "punctuator .";
            }
        } break;

        case ':':
        {
            if(total < file_length && cur_char() == '>')
            {
                ++column;
                ++total;
                kind = tok::colongreater;
                kind_str = "punctuator :>";
            }
            else
            {
                kind = tok::colon;
                kind_str = "punctuator :";
            }
        } break;

        case '>':
        {
            const unsigned char v = total < file_length ? cur_char() : 0;
            if(v == '=')
            {
                ++column;
                ++total;
                kind = tok::greaterequal;
                kind_str = "punctuator >=";
            }
            else if(v == '>')
            {
                ++column;
                ++total;

                kind = tok::greatergreater;
                kind_str = "punctuator >>";
                if(total < file_length && cur_char() == '=')
                {
                    ++column;
                    ++total;
                    kind = tok::greatergreaterequal;
                    kind_str = "punctuator >>=";
                }
            }
            else
            {
                kind = tok::greater;
                kind_str = "punctuator >";
            }
        } break;

        case '_':

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        {
            //after first char, we can have numbers as well
            constexpr const char* ids = "_"
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789";

            while(total < file_length && std::count(ids, ids + 63, cur_char()))
            { ++column; ++total; }

            //lex keywords in a smarter way
            std::string str(content + old_total, content + total);
            auto it = tok::str_to_token_kind.find(str);
            kind = it == tok::str_to_token_kind.end() ? tok::identifier : it->second;
            kind_str = (it == tok::str_to_token_kind.end() ? "identifier " : "keyword ") + str;
        } break;

        default:
        {
            throw_error(row, column, {content + old_total, content + total});
        } break;
        }

        if(print)
        {
            std::cout << filepath << ":"
                      << row      << ":"
                      << old_col  << ": "
                      << kind_str << "\n";
            continue;
        }
        
        const std::size_t tokaroff = (curtok++) * token_total_positions;
        tokarr[tokaroff + token_kind_pos]  = kind;
        tokarr[tokaroff + token_begin_pos] = reinterpret_cast<std::size_t>(content + old_total);
        tokarr[tokaroff + token_end_pos]   = reinterpret_cast<std::size_t>(content + total);
        tokarr[tokaroff + token_row_pos]   = row;
        tokarr[tokaroff + token_col_pos]   = old_col;
        ++tokarrsize;
    }
    curtok = 0U;
}
    
void lexer::throw_error(std::size_t row, std::size_t column, std::string tokstr) const
{
    throw std::invalid_argument(filepath + ":" + std::to_string(row)
                                + ":" + std::to_string(column) +
                                ": error: Couldn\'t lex \"" +
                                tokstr + "\"");    
}

std::tuple<bool, std::uintptr_t, std::size_t, std::size_t> lexer::find_comment()
{
    std::size_t r = row;
    std::size_t col = column;
    std::size_t tot = total;

    bool ends = false;
    auto* ptr = content + total;
    for(; ptr != content + file_length;)
    {
        const char cpy = *ptr;
        if(cpy == '*')
        {
            ++col;
            ++tot;
            ++ptr;
            ends = true;
        }
        else if(cpy == '/' && ends)
        {
            ++col;
            ++tot;
            break;
        }
        else
        {
            if(cpy == '\r')
            {
                ++tot;
                col = 1U;
                ++r;
                if((ptr + 1) != nullptr && *(ptr + 1) == '\n')
                {
                    ++ptr;
                    ++tot;
                }
            }
            else if(cpy == '\n')
            {
                ++tot;
                col = 1U;
                ++r;
            }
            else
            {
                ++col;
                ++tot;
            }
            ++ptr;
            ends = false;
        }
        if(!ends && ptr + 1 == content + file_length && (*ptr == '\n' || *ptr == '\r'))
            throw_error(r + 1, 1, {content + total - 2, ptr});
    }
    return std::make_tuple(ends && *ptr == '/', tot, r, col);
}

}
