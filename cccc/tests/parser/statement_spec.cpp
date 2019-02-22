#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(parser_statements, statement_ ## Y)

using namespace mhc4;

static const std::string f = "statement.c";
const test_suite ts(f);


MYTEST(compound_0)
{
    try
    {
        ts.parse_body("{}", 2);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compound_1)
{
    try
    {
        ts.parse_body("{1;}", 4);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compound_2)
{
    try
    {
        ts.parse_body("{1;2;\"hi\";}", 11);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compound_3)
{
    try
    {
        ts.parse_body("{label: return 0;}");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compound_4)
{
    try
    {
        ts.parse_body("{1; {{}} {2; 1;} return 0;}", 27);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(label_0)
{
    try
    {
        ts.parse_body("label: 1;", 9);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(label_1)
{
    try
    {
        ts.parse_body("label: {1; return 0;}", 21);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(label_2)
{
    try
    {
        ts.parse_body("label:;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(label_3)
{
    try
    {
        ts.parse_body("label:{int a;}");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(label_negative_0)
{
    try
    {
        ts.parse_body("label: int a;");
        ASSERT_FALSE(true) << "Statement required after label, not declaration";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(label_negative_1)
{
    try
    {
        ts.parse_body("int a: return 0;");
        ASSERT_FALSE(true) << "Label must be an identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(label_negative_2)
{
    try
    {
        ts.parse_body("int a;: return 0;");
        ASSERT_FALSE(true) << "Label must be an identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(label_negative_3)
{
    try
    {
        ts.parse_body(": return 0;");
        ASSERT_FALSE(true) << "Label needs an identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(label_negative_4)
{
    try
    {
        ts.parse_body("label: 15 + 1");
        ASSERT_FALSE(true) << "Statement required after label, not expression";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(expression_0)
{
    try
    {
        ts.parse_body("return 0;", 9);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(expression_1)
{
    try
    {
        ts.parse_body("1 == 1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(expression_2)
{
    try
    {
        // passes because while statement + null expression
        ts.parse_body("while(1){};");
        
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(expression_negative)
{
    try
    {
        ts.parse_body("if (1) then 1;; else 2;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(expression_null)
{
    try
    {
        ts.parse_body(";", 1);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_0)
{
    try
    {
        ts.parse_body("if (0);", 7);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_1)
{
    try
    {
        ts.parse_body("if(0){}", 7);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_2)
{
    try
    {
        ts.parse_body("if (0) return 0;", 16);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_3)
{
    try
    {
        ts.parse_body("if (1) 0;", 9);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_4)
{
    try
    {
      ts.parse_body("if (1) ; else;", 14);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_5)
{
    try
    {
        ts.parse_body("if (1 == 1) 1 == 1; else 1 == 2;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_6)
{
    try
    {
        ts.parse_body(
R"(
    if (1 < 2) {
        // do stuff
    }
    if (2 > 1) {
        // do more stuff
    }
    else {
        // i belong to if nr. 2
    }
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_7)
{
    try
    {
        ts.parse_body(
    R"(
        goto in_if;
        if (1 + 1) {
            in_if:
            return 0;
        }
        else {
            return 1;
        }
    )"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(selection_8)
{
    try
    {
        ts.parse_body(
R"(
    int x;
    if (x = 5) lab: return 0;
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_9)
{
    try
    {
        ts.parse_body(
R"(
    lab: if(1) {
        goto bal;
        alb: goto lab;
    }
    else {
        bal:
        goto alb;
    }
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_10)
{
    try
    {
        ts.parse_body("if (1) while (1) 1;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_11)
{
    try
    {
        ts.parse(
R"(
    int condition(int i);
    int main()
    {
        if (condition(1)) condition(1);
    }
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_12)
{
    try
    {
        ts.parse_body(
R"(
    // labeled statement
    if (1) label:;

    // compound statement
    if (1) {
        // declaration
        int a;
        // statements
        1;
        ;
        if(1);
    }

    // expression and null statements
    if (1) ;
    if (1) 1;
    if (1) 10 / (8 - 5) * 15;

    // selection statements
    if (1) if(1) ;;
    if (1) if(1); else;

    // iteration statements
    if (1) while(1);
    if (1) while(1) 1;

    // jump statements
    if (1) goto label;
    if (1) continue;
    if (1) break;
    if (1) return 0;
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_13)
{
    try
    {
        ts.parse_body(
R"(
    int a;

    // primary expression
    if(a);
    if(1);
    if("a");
    if((a));
    if((1));
    if(("a"));
    if((((((((1))))))));

    // postfix expression
    if(a[1]);
    if(a.a);
    if(a->a);
    if(a());
    if(a(a,a));

    // unary expression
    if(sizeof a);
    if(&a);
    if(*a);
    if(-a);
    if(!a);
    if(sizeof (int)); // <<--- this one causes this test to fail

    // mult expression
    if(1*a);
    if(1/a);
    if(1*1/1);

    // add expression
    if(1+a);
    if(1-a);
    if(1+1-1);

    // relational expression
    if(1<a);
    if(1>a);
    if(1<=a);
    if(1>=a);
    if(1<1>1<=1>=1);

    // equality expression
    if(1==a);
    if(1!=a);
    if(1==1!=1);

    // logical and expression
    if(1&&a);
    if(1&&1&&1);

    // logical or expression
    if(1||a);
    if(1||1||1);

    // conditional expression
    if(1?1:1);
    if(a?a?1:1:1);

    // assignment expression
    if(a = 1);
    if(a=a=1);
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_negative_0)
{
    try
    {
        ts.parse_body("if 1 {}", 7);
        ASSERT_FALSE(true) << "Parenthesis are missing after if statement";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_negative_1)
{
    try
    {
        ts.parse_body("if (1)", 6);
        ASSERT_FALSE(true) << "Missing expression after if statement";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}


MYTEST(selection_negative_2)
{
    try
    {
        ts.parse_body("if (1) ; else", 13);
        ASSERT_FALSE(true) << "Missing expression after else";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_negative_3)
{
    try
    {
        ts.parse_body("if (1) else;", 12);
        ASSERT_FALSE(true) << "Missing expression after if in if-else statement";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_negative_4)
{
    try
    {
        ts.parse_body("else;");
        ASSERT_FALSE(true) << "Missing an if statement before the else";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_negative_5)
{
    try
    {
        ts.parse_body("else 1;");
        ASSERT_FALSE(true) << "Missing an if statement before the else";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_negative_6)
{
    try
    {
        ts.parse_body("if () 1;");
        ASSERT_FALSE(true) << "Missing expression within the parenthesis";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_nested_0)
{
    try
    {
        ts.parse_body("if (1) if(2) ; else ;", 21);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_1)
{
    try
    {
        ts.parse_body("if (1) ; else if (0) ; else ;", 29);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_2)
{
    try
    {
        ts.parse_body("if (1) ; else {if (0) ; else ;}");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_3)
{
    try
    {
        ts.parse_body("if (0) if (1) if (2) ;;;", 24);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_4)
{
    try
    {
        ts.parse_body("if (0) {if (1) {if (2) ;};} else;;", 34);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_5)
{
    try
    {
        ts.parse_body(
R"(
    int a;
    int b;
    if(a == b) a = a + 1;
    if(b == a) {
        b = a - 1;
    }
    else if (b == 4) {
        a = 5;
    }
    else {
        if (1) {
            int c;
            c = b;
            b = 1;
            a = c;
        }
        if (0) {
            8;
        }
        else {
            {1;}
        }
    }
)"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(selection_nested_negative_0)
{
    try
    {
        ts.parse_body("if (1) if (1) if(1) ;;;else;", 28);
        ASSERT_FALSE(true) << "else scoping with if does not match";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_nested_negative_1)
{
    try
    {
        ts.parse_body("if (1) if (1) if 1 ;else;", 25);
        ASSERT_FALSE(true) << "missing parenthesis should not be allowed";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(selection_nested_negative_2)
{
    try
    {
        ts.parse_body(
R"(
    if (1) {
        else {
            return 0;
        }
    }
)"
        );
        ASSERT_FALSE(true) << "else is nested within the if statement";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(iteration_0)
{
    try
    {
        ts.parse_body("while (1);", 10);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(iteration_2)
{
  try
  {
    ts.parse_body("while (1) {\"hi\";}", 17);
  }
  catch(std::exception& e)
  {
    ASSERT_FALSE(true);
  }
}

MYTEST(iteration_3)
{
  try
  {
    ts.parse_body("while (1 == 1) {0; return 0;}", 29);
  }
  catch(std::exception& e)
  {
    ASSERT_FALSE(true) << e.what();
  }
}

MYTEST(iteration_negative_0)
{
    try
    {
        ts.parse_body("while 1 {}", 10);
        ASSERT_FALSE(true) << "No parenthesis around condition not allowed";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(iteration_negative_1)
{
    try
    {
        ts.parse_body("while (1)", 9);
        ASSERT_FALSE(true) << "No statement after condition in whilestatement!";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}


MYTEST(iteration_nested_0)
{
    try
    {
        ts.parse_body("while (1) while (2);", 20);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(iteration_nested_1)
{
    try
    {
        ts.parse_body("while (1) {while (1) ;}", 23);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(iteration_nested_negative_0)
{
    try
    {
        ts.parse_body("while while (1);", 16);
        ASSERT_FALSE(true) << "The condition for a while statement should not be missing";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(iteration_nested_negative_1)
{
    try
    {
        ts.parse_body("while (1) while (1)", 19);
        ASSERT_FALSE(true) << "The statement following a while should not be missing";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_goto_0)
{
    try
    {
        ts.parse_body("goto lab; lab: 1;", 17);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_goto_1)
{
    try
    {
        ts.parse_body("lab: 1; goto lab;", 17);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_goto_2)
{
    try
    {
        ts.parse_body("lab: while (1) {1; 2; goto lab;}", 32);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_goto_3)
{
    try
    {
        ts.parse_body("lab:goto lab2; lab2: goto lab;", 30);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_goto_4)
{
    try
    {
        ts.parse_body("goto into; if(1) {1;} else {into:2;}", 36);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_goto_negative_0)
{
    try
    {
        ts.parse_body("goto;");
        ASSERT_FALSE(true) << "goto needs identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_goto_negative_1)
{
    try
    {
        ts.parse_body("goto 1;");
        ASSERT_FALSE(true) << "goto needs identifier";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_continue_0)
{
    try
    {
      ts.parse_body("while (1) continue;", 19);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_continue_1)
{
    try
    {
      ts.parse_body("while (1) {continue;}", 21);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_continue_2)
{
    try
    {
      ts.parse_body("while (1) {{continue;}}", 23);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_continue_3)
{
    try
    {
      ts.parse_body("while (1) {1; {continue;} 0;}", 29);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_continue_4)
{
    try
    {
      ts.parse_body("while (1) { while (1) {continue;} continue;}", 44);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}



MYTEST(jump_break_0)
{
    try
    {
      ts.parse_body("while (0) break;", 16);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_break_1)
{
    try
    {
      ts.parse_body("while (1) {break;}", 18);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_break_2)
{
    try
    {
      ts.parse_body("while (1) {{break;}}", 20);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_break_3)
{
    try
    {
      ts.parse_body("while (1) {1; {break;} 0;}", 26);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_break_4)
{
    try
    {
      ts.parse_body("while (1) { while (1) {break;} break;}", 38);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(jump_return_0)
{
    try
    {
        ts.parse_body("return 0;", 9);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_return_1)
{
    try
    {
        ts.parse_body("return 1 + 1;", 13);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_return_2)
{
    try
    {
        ts.parse_body("if (0) return 0; else return 1;", 31);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_return_3)
{
    try
    {
        ts.parse("void foo(){return;} int main(){return 1;}", 41);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_return_negative_0)
{
    try
    {
        ts.parse_body("return return;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(jump_return_negative_1)
{
    try
    {
        ts.parse_body("return return 0;;");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(func_negative_0)
{
    try
    {
        ts.parse("int foo (int x,) {\nreturn;\n}");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        std::string str = e.what();
        //error location is the first problematic token
        // Note: for this example, the problematic token is ')'
        auto errloc = str.find("1:16");
        EXPECT_NE(errloc, std::string::npos) << e.what();
    }
}
