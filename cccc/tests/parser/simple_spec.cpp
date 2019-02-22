#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(parser_simple, simple_ ## Y)

using namespace mhc4;

static const std::string f = "simple.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(empty)
{
    try
    {
        ts.parse("", 0);
        ASSERT_TRUE(false) << "Parser must not accept an empty file";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(basic_body)
{
    try
    {
        ts.parse("int main(){}", 12);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_call)
{
    try
    {
        ts.parse("int foo() {return 0;} int main(){foo();}", 40);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_int_0)
{
    try
    {
        ts.parse("int x = 12345678901234567890;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_int_1)
{
    try
    {
        ts.parse("int x = -12345678901234567890;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_int_2)
{
    try
    {
        ts.parse("int x = 2147483648;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_int_3)
{
    try
    {
        ts.parse("int x = -2147483649;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_int_4)
{
    try
    {
        ts.parse_body(
            "int x;\nx = 99999999999;"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_char_0)
{
    try
    {
        ts.parse_body(
            "char c; c = -129;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
       ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(big_number_char_1)
{
    try
    {
        ts.parse_body(
            "char c; c = 128;"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_0)
{
    try
    {
        ts.parse_body("int x = 4894648;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_1)
{
    try
    {
        ts.parse_body("int x = -13459;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_2)
{
    try
    {
        ts.parse_body("int x = 0;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_3)
{
    try
    {
        ts.parse_body("char c; c = 127;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_4)
{
    try
    {
        ts.parse_body("char c; c = -128;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_5)
{
    try
    {
        ts.parse_body("int x = -0;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_6)
{
    try
    {
        ts.parse_body("char c; c = -0;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_7)
{
    try
    {
        ts.parse_body("int a = (1);");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(number_negative_0)
{
    try
    {
        ts.parse_body("int x = 0123;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_1)
{
    try
    {
        ts.parse_body("int x = 00000000;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_2)
{
    try
    {
        ts.parse_body("int x = -024;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_3)
{
    try
    {
        ts.parse_body("char c = 010");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_4)
{
    try
    {
        ts.parse_body("int i = 123 456;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_5)
{
    try
    {
        ts.parse_body("1abc;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_6)
{
    try
    {
        ts.parse_body("1ABC;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_7)
{
    try
    {
        ts.parse_body("0x1;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_8)
{
    try
    {
        ts.parse_body("0.0;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_9)
{
    try
    {
        ts.parse_body(".0;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_10)
{
    try
    {
        ts.parse_body("1.5;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_11)
{
    try
    {
        ts.parse(
R"(
    int a = 012345;
    int B[012];
    int c(int n)
    {
        int d = 012;
    }
)"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_12)
{
    try
    {
        ts.parse_body(
            "int a = 123_456;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_13)
{
    try
    {
        ts.parse_body(
            "int a = 123f;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_14)
{
    try
    {
        ts.parse_body(
            "int a = 123F;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_15)
{
    try
    {
        ts.parse_body(
            "int a = 123L;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_16)
{
    try
    {
        ts.parse_body(
            "int a = 123l;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(number_negative_17)
{
    try
    {
        ts.parse_body(
            "int a = (1;"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(non_closed_block)
{
    try
    {
        ts.parse("int main() {return 0;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(digraph_0)
{
    try
    {
        ts.parse(
R"(int main() <%
    return 0;
%>)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << "digraphs are allowed";
    }
}

MYTEST(digraph_1)
{
    try
    {
        ts.parse(
R"(int main() <%
    <% int a;
    {a;}
    a;
    %>
    return 0;
%>)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << "digraphs are allowed";
    }
}

MYTEST(digraph_2)
{
    try
    {
        ts.parse(
R"(int main() <%
    int a<::>;
    char c[];
    return 0;
%>)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << "digraphs are allowed";
    }
}

MYTEST(digraph_3)
{
    try
    {
        ts.parse_body(
R"(
    int a<:];
    char b[:>;
    <% int c; }
    { char d; %>
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << "digraphs are allowed";
    }
}
