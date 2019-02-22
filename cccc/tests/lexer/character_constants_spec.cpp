#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_character_constants, charseq_ ## Y)

using namespace mhc4;

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

const std::string f = "character_constant.c";
const test_suite ts(f);

MYTEST(simple_0)
{
    auto vec = ts.lex_all("\'a\'", 3);
    EXPECT_EQ(ts.get_charconst_token("\'a\'", 1, 1), vec[0]);
}

MYTEST(simple_1)
{
    auto vec = ts.lex_all("\'c\'\'e\'", 6);
    EXPECT_EQ(ts.get_charconst_token("\'c\'", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_charconst_token("\'e\'", 1, 4), vec[1]);
}

MYTEST(escape_seq_0)
{
    auto vec = ts.lex_all("\'\\\'\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\\'\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_1)
{
    auto vec = ts.lex_all("\'\\\"\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\\"\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_2)
{
    auto vec = ts.lex_all("\'\\?\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\?\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_3)
{
    auto vec = ts.lex_all("\'\\\\\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\\\\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_4)
{
    auto vec = ts.lex_all("\'\\a\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\a\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_5)
{
    auto vec = ts.lex_all("\'\\b\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\b\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_6)
{
    auto vec = ts.lex_all("\'\\f\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\f\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_7)
{
    auto vec = ts.lex_all("\'\\n\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\n\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_8)
{
    auto vec = ts.lex_all("\'\\r\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\r\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_9)
{
    auto vec = ts.lex_all("\'\\t\'", 4);
    EXPECT_EQ(ts.get_charconst_token("\'\\t\'", 1, 1), vec[0]);
}

MYTEST(escape_seq_90)
{
  auto vec = ts.lex_all("\'\\v\'", 4);
  EXPECT_EQ(ts.get_charconst_token("\'\\v\'", 1, 1), vec[0]);
}

MYTEST(invalid_escape_seq)
{
    try
    {
        ts.lex_all("\'\\\'", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\'\\\'\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:3: error: Couldn\'t lex \"\'\\\'\"", arg.what());
    }
}

MYTEST(invalid_char_constant_0)
{
    try
    {
        ts.lex_all("\'\n\'", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\'\n\'\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:1: error: Couldn\'t lex \"\'\"", arg.what());
    }
}

MYTEST(invalid_char_constant_1)
{
    try
    {
        ts.lex_all("\'Λ\'", 4);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\'Λ\'\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"\'\xce\"", arg.what());
    }
}

MYTEST(invalid_char_constant_2)
{
    try
    {
        ts.lex_all("\'ſ\'", 4);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\'ſ\'\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"\'\xC5\"", arg.what());
    }
}

MYTEST(none)
{
    try
    {
      ts.lex_all("''", 2);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"''\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"''\"", arg.what());
    }
}
