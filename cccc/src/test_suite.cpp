#ifdef __WE_TEST_NOW_WITH_GOOGLETEST__

#include <parse/parser.hpp>
#include <lex/lexer.hpp>
#include <sema/analyzer.hpp>
#include <test_suite.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

test_suite::test_suite(const std::string& f_path) : f_path(f_path)
{ }

std::vector<token> test_suite::lex_all(const char* to_lex, std::size_t len) const
{
      std::vector<token> v;
      lexer lex(f_path, to_lex, len);
      for(token tok = lex.lex(); tok.is_eof(); tok = lex.lex())
          v.push_back(tok);
      return v;
}

std::vector<token> test_suite::lex_all(const std::string& to_lex) const
{
    return lex_all(to_lex.c_str(), to_lex.size());
}

void test_suite::parse(const char* to_parse, std::size_t len) const
{
    lexer lex(f_path, to_parse, len);
    parser par(lex);
    par.parse();
}

void test_suite::parse_body(const char* to_parse, std::size_t len) const
{
    std::string str("int main(){\n");
    str.append(to_parse);
    str.append("\n}");

    assert(str.size() == (len + 14) && "input length does not match");
    parse(str.c_str(), len + 14);
}

void test_suite::parse(const std::string& str) const
{
    parse(str.c_str(), str.size());
}

void test_suite::parse_body(const std::string& str) const
{
    parse_body(str.c_str(), str.size());
}

void test_suite::analyze(const std::string& to_analyze) const
{
    lexer lex(f_path, to_analyze.c_str(), to_analyze.size());
    parser par(lex);
    par.parse();

    const auto& ast = par.ast();
    semantics::analyze(ast);
}

void test_suite::analyze_body(const std::string& to_analyze) const
{
    std::string str("int main(){\n");
    str.append(to_analyze);
    str.append("return 0;\n}");
    analyze(str);
}

bool test_suite::expect_err_loc(std::exception& e, int row, int colomn, bool in_body) const
{
    if (in_body) row++;
    std::string got = e.what();
    std::string expect(":");
    expect.append(std::to_string(row));
    expect.append(":");
    expect.append(std::to_string(colomn));
    expect.append(": error:");
    auto it = got.find(expect);
    return !(it == std::string::npos);
}

token test_suite::get_string_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::string_literal, str);
}

token test_suite::get_identifier_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::identifier, str);
}

token test_suite::get_charconst_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::char_constant, str);
}

token test_suite::get_numeric_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::numeric_constant, str);
}

token test_suite::get_punctuator_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::from_string(str), str);
}

token test_suite::get_keyword_token(const std::string& str, int row, int colomn) const
{
    return token(source_location(f_path, row, colomn), tok::from_string(str), str);
}

}

#endif
