#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_punctuators, punctuator_ ## Y)

using namespace mhc4;

static const std::string f = "punctuators.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(simple_0)
{
    auto vec = ts.lex_all("+", 1);
    EXPECT_EQ(ts.get_punctuator_token("+", 1, 1), vec[0]);
}

MYTEST(simple_1)
{
    auto vec = ts.lex_all("-+", 2);
    EXPECT_EQ(ts.get_punctuator_token("-", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token("+", 1, 2), vec[1]);
}

MYTEST(pair)
{
    auto vec = ts.lex_all("> >", 3);
    EXPECT_EQ(ts.get_punctuator_token(">", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token(">", 1, 3), vec[1]);
}

MYTEST(two_character)
{
    auto vec = ts.lex_all("<<", 2);
    EXPECT_EQ(ts.get_punctuator_token("<<", 1, 1), vec[0]);
}

MYTEST(three_character)
{
    auto vec = ts.lex_all("...", 3);
    EXPECT_EQ(ts.get_punctuator_token("...", 1, 1), vec[0]);
}

MYTEST(period_numeric_combination)
{
    auto vec = ts.lex_all("...3", 4);
    EXPECT_EQ(ts.get_punctuator_token("...", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_numeric_token("3", 1, 4), vec[1]);
}

MYTEST(two)
{
    auto vec = ts.lex_all("=/", 2);
    EXPECT_EQ(ts.get_punctuator_token("=", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token("/", 1, 2), vec[1]);
}

MYTEST(two_dots)
{
    auto vec = ts.lex_all("..", 2);
    EXPECT_EQ(ts.get_punctuator_token(".", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token(".", 1, 2), vec[1]);
}

MYTEST(complex)
{
    auto vec = ts.lex_all(";..(=!+++=--><:||-==", 20);
    EXPECT_EQ(ts.get_punctuator_token(";", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token(".", 1, 2), vec[1]);
    EXPECT_EQ(ts.get_punctuator_token(".", 1, 3), vec[2]);
    EXPECT_EQ(ts.get_punctuator_token("(", 1, 4), vec[3]);
    EXPECT_EQ(ts.get_punctuator_token("=", 1, 5), vec[4]);
    EXPECT_EQ(ts.get_punctuator_token("!", 1, 6), vec[5]);
    EXPECT_EQ(ts.get_punctuator_token("++", 1, 7), vec[6]);
    EXPECT_EQ(ts.get_punctuator_token("+=", 1, 9), vec[7]);
    EXPECT_EQ(ts.get_punctuator_token("--", 1, 11), vec[8]);
    EXPECT_EQ(ts.get_punctuator_token(">", 1, 13), vec[9]);
    EXPECT_EQ(ts.get_punctuator_token("<:", 1, 14), vec[10]);
    EXPECT_EQ(ts.get_punctuator_token("||", 1, 16), vec[11]);
    EXPECT_EQ(ts.get_punctuator_token("-=", 1, 18), vec[12]);
    EXPECT_EQ(ts.get_punctuator_token("=", 1, 20), vec[13]);
}
