#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(semantic_constraint, constraint_ ## Y)

using namespace mhc4;

static const std::string f = "sem_constraint.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}

MYTEST(goto_negative_0)
{
    try
    {
        ts.analyze("int foo(){label: 1;} int main() {goto label;}");
        ASSERT_FALSE(true) << "Label must be in the enclosing function";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(goto_negative_1)
{
    try
    {
        ts.analyze_body("goto label; {int a; a = 0; label: a = 2;}");
        ASSERT_FALSE(true) << "A goto statement is not allowed to jump past any"
                              "declarations of objects with variably modified types";
                              // Quote: ISO/IEC 9899:201x Page 153 §6.8.6.3 4 EXAMPLE 2
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(continue_negative_0)
{
    try
    {
        ts.analyze_body("continue;");
        ASSERT_FALSE(true) << "\"continue\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(continue_negative_1)
{
    try
    {
        ts.analyze_body("while (1) 1; continue;");
        ASSERT_FALSE(true) << "\"continue\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(continue_negative_2)
{
    try
    {
        ts.analyze_body("{continue;}");
        ASSERT_FALSE(true) << "\"continue\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(break_negative_0)
{
    try
    {
        ts.analyze_body("break;");
        ASSERT_FALSE(true) << "\"break\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(break_negative_1)
{
    try
    {
        ts.analyze_body("while (1) 1; break;");
        ASSERT_FALSE(true) << "\"break\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(break_negative_2)
{
    try
    {
        ts.analyze_body("{break;}");
        ASSERT_FALSE(true) << "\"break\" must be in the body of a loop";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

// 6.8.6.4 Constraints
//A return statement with an expression shall not appear in a function whose
//return type is void. A return statement without an expression shall only
//appear  in  a  function whose return type is void.

MYTEST(return_negative_0)
{
    try
    {
        ts.analyze("void foo(){return 1;} int main(){}");
        ASSERT_FALSE(true) << "return with an expression is not allowed in an void function";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(return_negative_1)
{
    try
    {
        ts.analyze_body("return;");
        ASSERT_FALSE(true) << "return without expression is not allowed in an non-void function";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}
