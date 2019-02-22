#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_strings, string_ ## Y)

using namespace mhc4;

static const std::string f = "strings.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(empty)
{
    auto vec = ts.lex_all("\"\"", 2);
    EXPECT_EQ(ts.get_string_token("\"\"", 1, 1), vec[0]);
}

MYTEST(single_char)
{
    auto vec = ts.lex_all("\"a\"", 3);
    EXPECT_EQ(ts.get_string_token("\"a\"", 1, 1), vec[0]);
}

MYTEST(two_chars)
{
    auto vec = ts.lex_all("\"aa\"", 4);
    EXPECT_EQ(ts.get_string_token("\"aa\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_0)
{
    auto vec = ts.lex_all("\"abcdefghijklmnopqrstuvwxyz\"", 28);
    EXPECT_EQ(ts.get_string_token("\"abcdefghijklmnopqrstuvwxyz\"", 1, 1),
              vec[0]);
}

MYTEST(multiple_characters_1)
{
    auto vec = ts.lex_all("\"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"", 28);
    EXPECT_EQ(ts.get_string_token("\"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"", 1, 1),
              vec[0]);
}

MYTEST(multiple_characters_2)
{
    auto vec = ts.lex_all("\"äöüÄÖÜ_[]^!<>=&ß:-\"", 27);
    EXPECT_EQ(ts.get_string_token("\"äöüÄÖÜ_[]^!<>=&ß:-\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_3)
{
    auto vec = ts.lex_all("\"äöüÄÖÜ_[]^!<>=&ß:-\"", 27);
    EXPECT_EQ(ts.get_string_token("\"äöüÄÖÜ_[]^!<>=&ß:-\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_4)
{
    auto vec = ts.lex_all("\"$€^¡¢£¤¥¦§¨©ª«¬\"", 31);
    EXPECT_EQ(ts.get_string_token("\"$€^¡¢£¤¥¦§¨©ª«¬\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_5)
{
    auto vec = ts.lex_all("\"°±²³´µ¶·¸¹º»¼½¾¿\"", 34);
    EXPECT_EQ(ts.get_string_token("\"°±²³´µ¶·¸¹º»¼½¾¿\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_6)
{
    auto vec = ts.lex_all("\"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ\"", 34);
    EXPECT_EQ(ts.get_string_token("\"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_7)
{
    auto vec = ts.lex_all("\"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß\"", 34);
    EXPECT_EQ(ts.get_string_token("\"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_8)
{
    auto vec = ts.lex_all("\"àáâãäåæçèéêëìíîï\"", 34);
    EXPECT_EQ(ts.get_string_token("\"àáâãäåæçèéêëìíîï\"", 1, 1), vec[0]);
}

MYTEST(multiple_characters_9)
{
    auto vec = ts.lex_all("\"ðñòóôõö÷øùúûüýþÿ\"", 34);
    EXPECT_EQ(ts.get_string_token("\"ðñòóôõö÷øùúûüýþÿ\"", 1, 1), vec[0]);
}

MYTEST(escape_sequences)
{
    auto vec = ts.lex_all("\"\\n\\a\\\"\\?\\f\\r\\b\\t\\v\"", 20);
    EXPECT_EQ(ts.get_string_token("\"\\n\\a\\\"\\?\\f\\r\\b\\t\\v\"", 1, 1),
              vec[0]);
}

MYTEST(two_backslashes)
{
    auto vec = ts.lex_all("\"\\\\\"", 4);
    EXPECT_EQ(ts.get_string_token("\"\\\\\"", 1, 1), vec[0]);
}

MYTEST(unescaped_backslash)
{
    try
    {
        ts.lex_all("\"\\\"", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\\\"\\\\\"\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:3: error: Couldn\'t lex \"\"\\\"\"", arg.what());
    }
}

MYTEST(not_closed_0)
{
    try
    {
        ts.lex_all("\"", 1);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\\\"\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:1: error: Couldn\'t lex \"\"\"", arg.what());
    }
}

MYTEST(not_closed_1)
{
    try
    {
        ts.lex_all("\"I am not a terminated string", 29);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"I am not a terminated string";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:29: error: Couldn\'t lex \"\"I am not a terminated string\"", arg.what());
    }
}

MYTEST(not_closed_2)
{
    try
    {
        ts.lex_all("\" ", 2);
        ASSERT_FALSE(true) << "Expected lexing to fail for \" ";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"\" \"", arg.what());
    }
}

MYTEST(not_closed_3)
{
    try
    {
        ts.lex_all("\" ", 2);
        ASSERT_FALSE(true) << "Expected lexing to fail for \" ";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"\" \"", arg.what());
    }
}

MYTEST(space)
{
    auto vec = ts.lex_all("\" \"", 3);
    EXPECT_EQ(ts.get_string_token("\" \"", 1, 1), vec[0]);
}

MYTEST(line_break)
{
    try
    {
        ts.lex_all("\"\n\"", 3);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"\\\"\\n\\\"\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:1: error: Couldn\'t lex \"\"\"", arg.what());
    }
}

MYTEST(form_feed_out)
{
    auto vec = ts.lex_all("\"\f\"", 3);
    EXPECT_EQ(ts.get_string_token("\"\f\"", 1, 1), vec[0]);
}

MYTEST(form_feed)
{
    auto vec = ts.lex_all("\"\\f\"", 4);
    EXPECT_EQ(ts.get_string_token("\"\\f\"", 1, 1), vec[0]);
}

MYTEST(unterminated)
{
    try
    {
        ts.lex_all("a\"", 2);
        ASSERT_FALSE(true) << "Expected lexing to fail for \"a\\\"\"";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:2: error: Couldn\'t lex \"\"\"", arg.what());
    }
}

MYTEST(with_pseudo_comment)
{
    auto vec = ts.lex_all("\"Hello /* */ World!\"", 20);
    EXPECT_EQ(ts.get_string_token("\"Hello /* */ World!\"", 1, 1), vec[0]);
}

MYTEST(with_char_FF)
{
    auto vec = ts.lex_all("\"here goes the \xFF\"", 17);
    EXPECT_EQ(ts.get_string_token("\"here goes the \xFF\"", 1, 1), vec[0]);
}

// QUESTION All of the following tests fail at the moment, is this behavior in
// reltion to the specification intended?

MYTEST(with_chars_multiple)
{
    auto vec = ts.lex_all("\"\xAA\xAE\xFF\xBE\x03\x7A\"", 8);
    EXPECT_EQ(ts.get_string_token("\"\xAA\xAE\xFF\xBE\x03\x7A\"", 1, 1), vec[0]);
}

MYTEST(combination)
{
  try
  {
    ts.lex_all("\"\\\\,\n\f\rhi __  \"", 16);
    ASSERT_TRUE(false);
  }
  catch(std::invalid_argument& arg)
  {
      ASSERT_TRUE(true);
      //EXPECT_EQ(f + ":1:1: error: Couldn\'t lex \"\"", arg.what());
  }
}

MYTEST(formatting_within)
{
    auto vec = ts.lex_all("\"Hallo\\n\\tWelt!\\rHere\\fgoes!\"", 29);
    EXPECT_EQ(ts.get_string_token("\"Hallo\\n\\tWelt!\\rHere\\fgoes!\"", 1, 1), vec[0]);
}
