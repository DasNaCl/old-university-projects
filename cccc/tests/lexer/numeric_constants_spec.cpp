#include <test_suite.hpp>

#include <gtest/gtest.h>

#include <string>

#define MYTEST(Y) TEST(lexer_numeric_constants, numeric_ ## Y)

using namespace mhc4;

static const std::string f = "numeric_constant.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(simple)
{
    auto vec = ts.lex_all("1234567890\n", 11);
    EXPECT_EQ(ts.get_numeric_token("1234567890", 1, 1), vec[0]);
}

MYTEST(long_num)
{
  auto vec = ts.lex_all("1111111111111111111111111111111111111111111111111"
                        "1111111111111111111111111111111111111111111111111"
                        "1111111111111111111111111111111111111111111111111"
                        "1111111111111111111111111111111111111111111111111"
                        "1111111111111111111111111111111111111111111111111\n", 246);
    EXPECT_EQ(ts.get_numeric_token(
                    "1111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111"
                    "1111111111111111111111111111111111111111111111111", 1, 1),
                    vec[0]);
}

MYTEST(neg)
{
    auto vec = ts.lex_all("-99", 3);
    EXPECT_EQ(ts.get_punctuator_token("-", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_numeric_token("99", 1, 2), vec[1]);
}

MYTEST(zero)
{
    auto vec = ts.lex_all("0", 1);
    EXPECT_EQ(ts.get_numeric_token("0", 1, 1), vec[0]);
}
