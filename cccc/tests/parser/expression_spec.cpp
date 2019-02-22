#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(parser_expression, expression_ ## Y)

using namespace mhc4;

static const std::string f = "exp.c";
const test_suite ts(f);

MYTEST(primary_numeric)
{
    try
    {
        ts.parse_body("3;", 2);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(primary_string)
{
    try
    {
        ts.parse_body("\"string literal\";", 17);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(primary_identifier_0)
{
    try
    {
        ts.parse_body("a;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(primary_identifier_1)
{
    try
    {
        ts.parse_body("int a; a;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(parenthesis_0)
{
    try
    {
        ts.parse_body("(1);", 4);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(parenthesis_1)
{
    try
    {
        ts.parse_body("(\"hi\");", 7);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(parenthesis_2)
{
    try
    {
        ts.parse_body("(\'a\');", 6);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}
MYTEST(parenthesis_3)
{
    try
    {
        ts.parse_body("( 1 + (\"a\"));");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(parenthesis_negative_0)
{
    try
    {
        ts.parse_body("();", 3);
        ASSERT_FALSE(true) << "Expected to fail for empty parenthesis \"();\"";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(parenthesis_negative_1)
{
    try
    {
        ts.parse_body("(;", 2);
        ASSERT_FALSE(true) << "xpected to fail for not closed parenthesis \"(;\"";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(parenthesis_negative_2)
{
    try
    {
        ts.parse_body(");", 2);
        ASSERT_FALSE(true) << "xpected to fail for not opened parenthesis \");\"";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(parenthesis_negative_3)
{
    try
    {
        ts.parse_body("(());", 5);
        ASSERT_FALSE(true) << "Parenthesis need an expression within";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(unary_0)
{
    try
    {
        ts.parse_body("-1;", 3);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_1)
{
    try
    {
        ts.parse_body("*a;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_2)
{
    try
    {
        ts.parse_body("sizeof 1;", 9);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_3)
{
    try
    {
        ts.parse_body("&a;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_4)
{
    try
    {
        ts.parse_body("&1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_5)
{
    try
    {
        ts.parse_body("&**&1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(unary_negative_0)
{
    try
    {
        ts.parse_body("sizeof;");
        ASSERT_FALSE(true) << "unary sizeof cannot stand alone";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(unary_negative_1)
{
    try
    {
        ts.parse_body("-;");
        ASSERT_FALSE(true) << "unary - cannot stand alone";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(unary_negative_2)
{
    try
    {
        ts.parse_body("*;");
        ASSERT_FALSE(true) << "unary * cannot stand alone";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(unary_multiple_0)
{
    try
    {
        ts.parse_body("sizeof sizeof 1;");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(unary_multiple_1)
{
    try
    {
        ts.parse_body("sizeof -1;");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(unary_multiple_2)
{
    try
    {
        ts.parse_body("sizeof sizeof \"hi\" (1 == 1);");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(unary_multiple_3)
{
    try
    {
        ts.parse_body("*--a;");
    }
    catch(std::exception& e)
    {
      ASSERT_TRUE(false);
    }
}

MYTEST(unary_multiple_negative_0)
{
    try
    {
        ts.parse_body("**sizeof;");
        ASSERT_FALSE(true) << "unary operators need a cast expression thereafter";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(unary_multiple_negative_1)
{
    try
    {
        ts.parse_body("*1*;");
        ASSERT_FALSE(true) << "unary operators need a cast expression thereafter";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(negation_0)
{
    try
    {
        ts.parse_body("!1;", 3);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(negation_1)
{
    try
    {
        ts.parse_body("!(1);", 5);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(negation_2)
{
    try
    {
        ts.parse_body("!((1));", 7);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(negation_3)
{
    try
    {
        ts.parse_body("!a;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(negation_4)
{
    try
    {
        ts.parse_body("!\"i did not know this would compile\";", 37);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(negation_5)
{
    try
    {
        ts.parse_body("!!1;", 4);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(condidional_0)
{
    try
    {
        ts.parse_body("1 ? 2 : 3;", 10);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(condidional_1)
{
    try
    {
        ts.parse_body("1 ? \"hi\" : \"ho\";", 16);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(condidional_2)
{
    try
    {
        ts.parse_body("1 ? 1 : 2 ? 3 : 1;", 18);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(condidional_3)
{
    try
    {
        ts.parse_body("1 ? 1 ? 2 : 2 : 3;", 18);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(condidional_4)
{
    try
    {
        ts.parse_body("\"hi\" ? \'c\' : \'\\n\';", 18);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(conditional_5)
{
    try
    {
        ts.parse_body("1 ? 1 : 1; \"hi\";");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(conditional_6)
{
    try
    {
        ts.parse_body("1 ? 1 : 1 ? 1 : \"hi\";");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(conditional_7)
{
    try
    {
        ts.parse_body("1 ? 1 : (1 + \"hi\");");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}



MYTEST(binary_0)
{
    try
    {
        ts.parse_body("3 + 2;", 6);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(binary_1)
{
    try
    {
        ts.parse_body("1 + 2 + 3;", 10);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(binary_2)
{
    try
    {
        ts.parse_body("1 + 2 + 3 + 4;", 14);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(binary_3)
{
    try
    {
        ts.parse_body("1 + 2 - 3;", 10);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(binary_operators)
{
    try
    {
        ts.parse_body("1 * 2;", 6);
        ts.parse_body("1 + 2;", 6);
        ts.parse_body("1 - 2;", 6);
        ts.parse_body("1 < 2;", 6);
        ts.parse_body("1 == 2;", 7);
        ts.parse_body("1 != 2;", 7);
        ts.parse_body("1 && 2;", 7);
        ts.parse_body("1 || 2;", 7);
        ts.parse_body("a = 1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(assign_0)
{
    try
    {
        ts.parse_body("a || (b = c);");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(assign_1)
{
    try
    {
        ts.parse_body("1 = 1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(assign_2)
{
    try
    {
        ts.parse_body("&1 = *1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(assign_3)
{
    try
    {
        ts.parse_body("sizeof 1 = 1 + 1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(assign_4)
{
    try
    {
        ts.parse_body("int a; sizeof (int) *a = &a + -2;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}


MYTEST(sqr_brackets_0)
{
    try
    {
        ts.parse_body("a[0];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(sqr_brackets_1)
{
    try
    {
        ts.parse_body("a[0 + 1];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(sqr_brackets_2)
{
    try
    {
        ts.parse_body("a[foo()];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(sqr_brackets_3)
{
    try
    {
        ts.parse_body("a[0][1];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(sqr_brackets_4)
{
    try
    {
        ts.parse_body("a[b][c];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(sqr_brackets_negative_0)
{
    try
    {
        ts.parse_body("a[];");
        ASSERT_FALSE(true) << "Within the square brackets must be an expression";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(sqr_brackets_negative_1)
{
    try
    {
        ts.parse_body("a[;];");
        ASSERT_FALSE(true) << "Only expressions are allowed in square brackets, but ; is an expression-statement";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(sqr_brackets_negative_2)
{
    try
    {
        ts.parse_body("a[0;");
        ASSERT_FALSE(true) << "Square brackets must be closed";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(sqr_brackets_negative_3)
{
    try
    {
        ts.parse_body("a0];");
        ASSERT_FALSE(true) << "Square brackets must be opened";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(sqr_brackets_negative_4)
{
    try
    {
        ts.parse_body("[0]a;");
        ASSERT_FALSE(true) << "wrong order";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(false) << e.what();
    }
}

MYTEST(period_0)
{
    try
    {
        ts.parse_body("movie.number;\nmovie.name;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(period_1)
{
    try
    {
        ts.parse_body("0.movie;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(period_2)
{
    try
    {
        ts.parse_body("\"i am a mighty postfix expression\".do_stuff;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(period_negative_0)
{
    try
    {
        ts.parse_body("movie.0;");
        ASSERT_TRUE(false) << "postfix_exp.identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(period_negative_1)
{
    try
    {
        ts.parse_body("0.\"this way is not allowed\";");
        ASSERT_FALSE(true) << "postfix_exp.identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}


MYTEST(arrow_0)
{
    try
    {
      ts.parse_body("movie->number;\nmovie->name;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(arrow_1)
{
    try
    {
        ts.parse_body("0->movie;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(arrow_2)
{
    try
    {
        ts.parse_body("\"i am a primary expression\"->movie;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(arrow_negative_0)
{
    try
    {
        ts.parse_body("movie->0;");
        ASSERT_FALSE(true) << "postfix_exp.identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(arrow_negative_1)
{
    try
    {
        ts.parse_body("movie->\"nope, not allowed\";");
        ASSERT_FALSE(true) << "postfix_exp.identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(postfix_combination_0)
{
    try
    {
        ts.parse_body("\"string\"[\"string\"];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}


MYTEST(postfix_combination_1)
{
    try
    {
        ts.parse_body("1[2][3][4[5[6 ? a[e] : \"the world is flat!\"]]];");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

