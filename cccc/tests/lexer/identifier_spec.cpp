#include <lex/source_location.hpp>
#include <lex/lexer.hpp>
#include <lex/token.hpp>
#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_identifiers, identifier_ ## Y)

using namespace mhc4;

static const std::string f = "identifiers.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(basic_0)
{
    auto vec = ts.lex_all("a", 1);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
}

MYTEST(basic_1)
{
    auto vec = ts.lex_all("abcdefgh", 8);
    EXPECT_EQ(ts.get_identifier_token("abcdefgh", 1, 1), vec[0]);
}

MYTEST(basic_2)
{
    auto vec = ts.lex_all("StunFNneobH", 11);
    EXPECT_EQ(ts.get_identifier_token("StunFNneobH", 1, 1), vec[0]);
}

MYTEST(basic_with_numbers_0)
{
    auto vec = ts.lex_all("abc123", 6);
    EXPECT_EQ(ts.get_identifier_token("abc123", 1, 1), vec[0]);
}

MYTEST(basic_with_numbers_1)
{
    auto vec = ts.lex_all("ab4224ba", 8);
    EXPECT_EQ(ts.get_identifier_token("ab4224ba", 1, 1), vec[0]);
}

MYTEST(basic_with_numbers_2)
{
    auto vec = ts.lex_all("3a3", 3);
    EXPECT_EQ(ts.get_numeric_token("3", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("a3", 1, 2), vec[1]);
}

MYTEST(underscore_0)
{
    auto vec = ts.lex_all("_", 1);
    EXPECT_EQ(ts.get_identifier_token("_", 1, 1), vec[0]);
}

MYTEST(underscore_1)
{
    auto vec = ts.lex_all("abc_cba", 7);
    EXPECT_EQ(ts.get_identifier_token("abc_cba", 1, 1), vec[0]);
}

MYTEST(complex)
{
    auto vec = ts.lex_all("_nui82SN__SN0hw2u11111111", 25);
    EXPECT_EQ(ts.get_identifier_token("_nui82SN__SN0hw2u11111111", 1, 1), vec[0]);
}

MYTEST(two_divided_by_CRLF)
{
    auto vec = ts.lex_all("_\r\n_", 4);
    EXPECT_EQ(ts.get_identifier_token("_", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("_", 2, 1), vec[1]);
}

MYTEST(negative_0)
{
    auto vec = ts.lex_all("12bez", 5);
    EXPECT_NE(ts.get_identifier_token("12bez", 1, 1), vec[0]);
}

MYTEST(negative_1)
{
    auto vec = ts.lex_all("12_", 3);
    EXPECT_NE(ts.get_identifier_token("12_", 1, 1), vec[0]);
}

MYTEST(negative_2)
{
    auto vec = ts.lex_all("Bez-ez", 6);
    EXPECT_NE(ts.get_identifier_token("Bez-ez", 1, 1), vec[0]);
}

MYTEST(negative_3)
{
    auto vec = ts.lex_all("ez+ze", 5);
    EXPECT_NE(ts.get_identifier_token("ez+ze", 1, 1), vec[0]);
}
