#include <lex/source_location.hpp>
#include <lex/lexer.hpp>
#include <lex/token.hpp>
#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(lexer_comments, comment_ ## Y)

using namespace mhc4;

static const std::string f = "comments.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(complex_0)
{
    auto vec = ts.lex_all("\"a//b\"\n"
                     "// */\n"
                     "f = g/**//h;\n"
                     "m = n//**/o\n"
                     "    + p;\n"
                     "// hai \\\n"
                     " bai \n", 62);
    EXPECT_EQ(ts.get_string_token("\"a//b\"", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("f", 3, 1), vec[1]);
    EXPECT_EQ(ts.get_punctuator_token("=", 3, 3), vec[2]);
    EXPECT_EQ(ts.get_identifier_token("g", 3, 5), vec[3]);
    EXPECT_EQ(ts.get_punctuator_token("/", 3, 10), vec[4]);
    EXPECT_EQ(ts.get_identifier_token("h", 3, 11), vec[5]);
    EXPECT_EQ(ts.get_punctuator_token(";", 3, 12), vec[6]);
    EXPECT_EQ(ts.get_identifier_token("m", 4, 1), vec[7]);
    EXPECT_EQ(ts.get_punctuator_token("=", 4, 3), vec[8]);
    EXPECT_EQ(ts.get_identifier_token("n", 4, 5), vec[9]);
    EXPECT_EQ(ts.get_punctuator_token("+", 5, 5), vec[10]);
    EXPECT_EQ(ts.get_identifier_token("p", 5, 7), vec[11]);
    EXPECT_EQ(ts.get_punctuator_token(";", 5, 8), vec[12]);
}

MYTEST(complex_1)
{
    auto vec = ts.lex_all("abc\n"
                     "// def\n"
                     "// ghi \\ /*\n"
                     "   jkl\n"
                     "// mno \\\n"
                     "   pqr\n"
                     "// stu \\\\"
                     "vwx", 58);
    EXPECT_EQ(ts.get_identifier_token("abc", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("jkl", 4, 4), vec[1]);
    ASSERT_TRUE(vec.size() < 3);
}

MYTEST(simple_0)
{
    try
    {
        auto vec = ts.lex_all("//", 2);
        ASSERT_TRUE(vec.empty());
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_FALSE(true) << "Did not expect lexing to fail for \"//\"";
    }
}

MYTEST(simple_1)
{
    auto vec = ts.lex_all("Hallo/**/Welt", 13);
    EXPECT_EQ(ts.get_identifier_token("Hallo", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("Welt", 1, 10), vec[1]);
}

MYTEST(simple_2)
{
    auto vec = ts.lex_all("konnichiha /***/ sekai", 22);
    EXPECT_EQ(ts.get_identifier_token("konnichiha", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("sekai", 1, 18), vec[1]);
}

MYTEST(simple_multiline_0)
{
    auto vec = ts.lex_all("a/* hi\n"
                          "bai */\nv", 15);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("v", 3, 1), vec[1]);
}

MYTEST(simple_multiline_1)
{
    auto vec = ts.lex_all("a// hi \\\n"
                          "bai\nv", 14);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("v", 3, 1), vec[1]);
}

MYTEST(simple_multiline_2)
{
    auto vec = ts.lex_all("a// hi \\\r"
                          "bai\nv", 14);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("v", 3, 1), vec[1]);
}

MYTEST(simple_multiline_3)
{
    auto vec = ts.lex_all("a// hi \\\r\n"
                          "bai\nv", 15);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("v", 3, 1), vec[1]);
}

MYTEST(simple_multiline_4)
{
    auto vec = ts.lex_all("//*All of this is a comment\nhi", 30);
    EXPECT_EQ(ts.get_identifier_token("hi", 2, 1), vec[0]);
}

MYTEST(simple_multiline_5)
{
    auto vec = ts.lex_all("//**/\n*/", 8);
    EXPECT_EQ(ts.get_punctuator_token("*", 2, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token("/", 2, 2), vec[1]);
}

MYTEST(not_closed)
{
    try
    {
        auto vec = ts.lex_all("hi /* lalala ");
        ASSERT_FALSE(true) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":1:13: error: Couldn\'t lex \"/* lalala \"", arg.what());
    }
}

MYTEST(not_closed_multiline_0)
{
    try
    {
        auto vec = ts.lex_all("/* hi\nbai *", 11);
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":2:5: error: Couldn\'t lex \"/* hi\nbai *\"", arg.what());
    }
}

MYTEST(not_closed_multiline_1)
{
    try
    {
        auto vec = ts.lex_all("/* hi\nbai", 9);
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        EXPECT_EQ(f + ":2:3: error: Couldn\'t lex \"/* hi\nbai\"", arg.what());
    }
}

MYTEST(not_closed_multiline_2)
{
    try
    {
        auto vec = ts.lex_all(
R"(int main() {}
    ha ha ha
    //123
    neari
    /* ehgaiunt
    nevfn)"
        );
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_TRUE(ts.expect_err_loc(arg, 6, 9)) << arg.what();
    }
}

MYTEST(not_closed_multiline_3)
{
    try
    {
        auto vec = ts.lex_all(
R"(int main() {}
    ha ha ha
    //123
    neari
    /* ehgaiunt
    nevfn
)"
        );
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_TRUE(ts.expect_err_loc(arg, 7, 1)) << arg.what();
    }
}

MYTEST(not_closed_multiline_4)
{
    try
    {
        auto vec = ts.lex_all(
R"(int main() {}
    ha ha ha
    //123
    neari
    /* ehgaiunt
    nevfn
 )"
        );
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_TRUE(ts.expect_err_loc(arg, 7, 1)) << arg.what();
    }
}

MYTEST(not_closed_multiline_5)
{
    try
    {
        auto vec = ts.lex_all(
R"(int main() {}
    ha ha ha
    //123
    neari
    /* ehgaiunt
    nevfn
 
)"
        );
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_TRUE(ts.expect_err_loc(arg, 8, 1)) << arg.what();
    }
}

MYTEST(not_closed_multiline_6)
{
    try
    {
        auto vec = ts.lex_all(
R"(int main() {}
    ha ha ha
    //123
    neari
    /* ehgaiunt
    nevfn
 
  )"
        );
        ASSERT_TRUE(false) << "Expected lexing to fail for unterminated comment";
    }
    catch(std::invalid_argument& arg)
    {
        ASSERT_TRUE(ts.expect_err_loc(arg, 8, 2)) << arg.what();
    }
}

MYTEST(multiline)
{
    auto vec = ts.lex_all("/*comment\n//pseudo end */\na*/b", 30);
    EXPECT_EQ(ts.get_identifier_token("a", 3, 1), vec[0]);
    EXPECT_EQ(ts.get_punctuator_token("*", 3, 2), vec[1]);
    EXPECT_EQ(ts.get_punctuator_token("/", 3, 3), vec[2]);
    EXPECT_EQ(ts.get_identifier_token("b", 3, 4), vec[3]);
}

MYTEST(multiline_with_CR)
{
    auto vec = ts.lex_all("a/*\rb\r*/\rc", 10);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("c", 4, 1), vec[1]);
}

MYTEST(multiline_with_CRLF)
{
    auto vec = ts.lex_all("a/*\r\nb\r\n*/\r\nc", 13);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_identifier_token("c", 4, 1), vec[1]);
}

MYTEST(inv_char_0)
{
    auto vec = ts.lex_all("ident 3 //Σ\nhello", 18);
    EXPECT_EQ(ts.get_identifier_token("ident", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_numeric_token("3", 1, 7), vec[1]);
    EXPECT_EQ(ts.get_identifier_token("hello", 2, 1), vec[2]);
}

MYTEST(inv_char_1)
{
    auto vec = ts.lex_all("ident 3 /*Σ*/hello", 19);
    EXPECT_EQ(ts.get_identifier_token("ident", 1, 1), vec[0]);
    EXPECT_EQ(ts.get_numeric_token("3", 1, 7), vec[1]);
    EXPECT_EQ(ts.get_identifier_token("hello", 1, 15), vec[2]);
}

MYTEST(inv_char_2)
{
    auto vec = ts.lex_all("a//Ξ⊂∀∃Π∪∩ℵΣΛ⇐",25);
    EXPECT_EQ(ts.get_identifier_token("a", 1, 1), vec[0]);
    ASSERT_TRUE(vec.size() == 1);
}
