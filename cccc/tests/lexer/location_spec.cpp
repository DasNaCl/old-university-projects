#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_locations, locations_ ## Y)

using namespace mhc4;

static const std::string f = "location.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(lf_0)
{
    auto vec = ts.lex_all("test\na", 6);
    EXPECT_EQ(ts.get_identifier_token("test", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("a", 2, 1), vec[1]);
}

MYTEST(lf_1)
{
    auto vec = ts.lex_all("\n 123", 5);
    EXPECT_EQ(ts.get_numeric_token("123", 2, 2), vec[0]);
}

MYTEST(cr_0)
{
    auto vec = ts.lex_all("hi\rho", 5);
    EXPECT_EQ(ts.get_identifier_token("hi", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("ho", 2, 1), vec[1]);
}

MYTEST(cr_1)
{
    auto vec = ts.lex_all("\r 123", 5);
    EXPECT_EQ(ts.get_numeric_token("123", 2, 2), vec[0]);
}

MYTEST(crlf_0)
{
    auto vec = ts.lex_all("Welt\r\nHallo", 11);
    EXPECT_EQ(ts.get_identifier_token("Welt", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("Hallo", 2, 1), vec[1]);
}

MYTEST(crlf_1)
{
    auto vec = ts.lex_all("\r\n 123", 6);
    EXPECT_EQ(ts.get_numeric_token("123", 2, 2), vec[0]);
}

MYTEST(column)
{
    auto vec = ts.lex_all("ab 12", 5);
    EXPECT_EQ(ts.get_identifier_token("ab", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_numeric_token("12", 1, 4), vec[1]);
}

MYTEST(row_0)
{
    auto vec = ts.lex_all("\nHello", 6);
    EXPECT_EQ(ts.get_identifier_token("Hello", 2, 1), vec[0]);
}

MYTEST(row_1)
{
    auto vec = ts.lex_all("\n\nHello\n\n", 8);
    EXPECT_EQ(ts.get_identifier_token("Hello", 3, 1), vec[0]);
}

MYTEST(fail_location_0)
{
    try
    {
        ts.lex_all("\n''", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"''\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":2:2: error: Couldn\'t lex \"''\"", arg.what());
    }
}

MYTEST(fail_location_1)
{
    try
    {
        ts.lex_all("Hallo =\n  ''", 12);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"''\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":2:4: error: Couldn\'t lex \"''\"", arg.what());
    }
}

MYTEST(fail_location_2)
{
    try
    {
        ts.lex_all(" ''", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"''\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:3: error: Couldn\'t lex \"''\"", arg.what());
    }
}
