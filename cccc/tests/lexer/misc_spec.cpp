#include <lex/source_location.hpp>
#include <lex/lexer.hpp>
#include <lex/token.hpp>
#include <test_suite.hpp>

#include <gtest/gtest.h>

#include <algorithm>

#define MYTEST(Y) TEST(lexer_misc, misc_ ## Y)

using namespace mhc4;

static const std::string f = "misc.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(all_toks)
{
    std::string keywords = std::string() +
#define KEYWORD(X) #X + "\n"
#include <lex/token_kind.def>
#undef KEYWORD
;

   std::string punctuators = std::string() +
#define PUNCTUATOR(X, Y) #Y + "\n"
#include <lex/token_kind.def>
#undef PUNCTUATOR
;

    auto vec_key = ts.lex_all(keywords);

    std::size_t pos_e = 0;
    int row = 0;
    std::string splitter = "\n";
    pos_e = keywords.find(splitter);
    while (pos_e != std::string::npos)
    {
        std::string sub = keywords.substr(0, pos_e);
        ASSERT_EQ(ts.get_keyword_token(sub, row + 1, 1), vec_key[row]);
        row++;
        keywords.erase(0, pos_e + splitter.length());
        pos_e = keywords.find(splitter);
    }

    punctuators.erase(std::remove(punctuators.begin(), punctuators.end(), '"'),
                      punctuators.end());
    auto vec_punct = ts.lex_all(punctuators);
    pos_e = 0;
    row = 0;
    pos_e = punctuators.find(splitter);
    while (pos_e != std::string::npos)
    {
        std::string sub = punctuators.substr(0, pos_e);
        ASSERT_EQ(ts.get_punctuator_token(sub, row + 1, 1), vec_punct[row]);
        row++;
        punctuators.erase(0, pos_e + splitter.length());
        pos_e = punctuators.find(splitter);
    }

    const std::string to_lex = std::string() +
        + "identifier" + "\n"
        + "1234567890" + "\n"
        + "\'A\'" + "\n"
        + "\"string_literal\"" + "\n"
        + "// A useful \\" + "\n"
        + "   comment" + "\n"
        + "/* OR\nWHAT\nEVER\n!!!!" + "\n"
        + "*****     ***********       */";

    auto vec = ts.lex_all(to_lex);
    ASSERT_EQ(ts.get_identifier_token("identifier", 1, 1), vec[0]);
    ASSERT_EQ(ts.get_numeric_token("1234567890", 2, 1), vec[1]);
    ASSERT_EQ(ts.get_charconst_token("\'A\'", 3, 1), vec[2]);
    ASSERT_EQ(ts.get_string_token("\"string_literal\"", 4, 1), vec[3]);
    ASSERT_TRUE(vec.size() == 4);
}

MYTEST(example_c_standard)
{
    auto vec = ts.lex_all(
      "void myfunc(void)\n"
      "{\n"
      "\t\tprintf(\"Hello World\\n\");\n"
      "\t\t/*...*/\n"
      "}\n", 59);
    EXPECT_EQ(ts.get_keyword_token("void", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("myfunc", 1, 6), vec[1]);
    EXPECT_EQ(ts.get_punctuator_token("(", 1, 12), vec[2]);
    EXPECT_EQ(ts.get_keyword_token("void", 1, 13), vec[3]);
    EXPECT_EQ(ts.get_punctuator_token(")", 1, 17), vec[4]);
    EXPECT_EQ(ts.get_punctuator_token("{", 2, 1), vec[5]);
    EXPECT_EQ(ts.get_identifier_token("printf", 3, 3), vec[6]);
    EXPECT_EQ(ts.get_punctuator_token("(", 3, 9), vec[7]);
    EXPECT_EQ(ts.get_string_token("\"Hello World\\n\"", 3, 10), vec[8]);
    EXPECT_EQ(ts.get_punctuator_token(")", 3, 25), vec[9]);
    EXPECT_EQ(ts.get_punctuator_token(";", 3, 26), vec[10]);
    EXPECT_EQ(ts.get_punctuator_token("}", 5, 1), vec[11]);
}

/*
MYTEST(simple_program)
{
    auto lex = lexer(f, "int main(int argc, char* argv[])\n"
                        "{\n"
                        "int a = 5;\n"
                        "printf(\"%%d\", a);\n"
                        "return 0;\n"
                        "}", 74);
}
*/

MYTEST(empty_0)
{
    ts.lex_all("", 0);
    ASSERT_TRUE(true);
}

MYTEST(empty_1)
{
    ts.lex_all(" \n\n\n\t\v\t\f\r\n\r\n\r", 13);
    ASSERT_TRUE(true);
}

MYTEST(empty_2)
{
    ts.lex_all("//// \\\\\nThisisacomment\n/* hello \n\n\n\r\n\r\nashdksahdk*/", 51);
    ASSERT_TRUE(true);
}

MYTEST(empty_3)
{
    auto lex = ts.lex_all(" ", 1);
    ASSERT_TRUE(true);
}
