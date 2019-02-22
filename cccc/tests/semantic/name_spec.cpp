#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(semantic_name, name_ ## Y)

using namespace mhc4;

static const std::string f = "sem_name.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}



MYTEST(simple_0)
{
    try
    {
        ts.analyze_body("int a; a;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(simple_1)
{
    try
    {
        ts.analyze_body("int b; {int a; a; b;} b;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(simple_2)
{
    try
    {
        ts.analyze_body("int a; a = 1; {char a; a = 'c';} a = 5;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(simple_negative_0)
{
    try
    {
        ts.analyze_body("a;");
        ASSERT_FALSE(true) << "Identifiers must be initialized";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 1, true)) << "Expected error at 1:1 - " << e.what();
    }
}

MYTEST(simple_negative_1)
{
    try
    {
        ts.analyze_body("{int a;} a;");
        ASSERT_FALSE(true) << "Identifiers must be initialized within their scope";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 10, true)) << "Expected error at 1:10 - " << e.what();
    }
}

MYTEST(goto_0)
{
    try
    {
        ts.analyze_body("label: 1;\n goto label;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(goto_1)
{
    try
    {
        ts.analyze_body("goto label;\nlabel:1;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(goto_negative_0)
{
    try
    {
        ts.analyze_body("goto label;");
        ASSERT_FALSE(true) << "Label for goto must be defined";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 6, true)) << "Expected error at 1:6 - " << e.what();
    }
}

MYTEST(goto_negative_1)
{
    try
    {
        ts.analyze_body("laba: 1;\nlaba: 2; goto laba;");
        ASSERT_FALSE(true) << "Label name is defined twice";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 1, true)) << "Expected error at 2:1 - " << e.what();
    }
}

MYTEST(goto_negative_2)
{
    try
    {
        ts.analyze_body("goto;");
        ASSERT_FALSE(true) << "No label given to goto";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 5, true)) << "Expected error at 1:5 - " << e.what();
    }
}

MYTEST(function_0)
{
    try
    {
        ts.analyze("int foo() {}\nint main(){foo(); return 0;}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_1)
{
    try
    {
        ts.analyze("int foo() {}\nint foobar(){foo();}\nint main(){foobar(); return 0;}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_2)
{
    try
    {
        ts.analyze(
R"(
    void a(){
        int abba;
        int habba;
        char labber;
    }

    void b(){
        char abba;
        int* habba;
        int labber;
    }

    int main(){
        a();
        b();
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

MYTEST(function_negative_0)
{
    try
    {
        ts.analyze_body("foo();");
        ASSERT_FALSE(true) << "Function must be declared";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 1, true)) << "Expected error at 1:1 - " << e.what();
    }
}

MYTEST(function_negative_1)
{
    try
    {
        ts.analyze_body("int foo; foo();");
        ASSERT_FALSE(true) << "function call on int";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 13, true)) << "Expected error at 1:13 - " << e.what();
    }
}

MYTEST(fuction_parameters_0)
{
    try
    {
        ts.analyze("int foo(int a){return a;}\nint main(){foo(1); return 0;}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(fuction_parameters_1)
{
    try
    {
        ts.analyze("int foo(int a, char b){a; return 1;}\nint main(){foo(1, 'c'); return 0;}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameters_negative_0)
{
    try
    {
        ts.analyze("int foo(){return a;}\nint main(){foo(a); return 0;}");
        ASSERT_FALSE(true) << "Function parameter a not defined";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 15)) << "Expected error at 2:12 - " << e.what();
    }
}

MYTEST(function_parameters_negative_1)
{
    try
    {
        ts.analyze("int foo(int a){return a;}\nint main(){foo(1); a; return 0;}");
        ASSERT_FALSE(true) << "Scope of a ends within foo.";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 20)) << "Expected error at 2:20 - " << e.what();
    }
}

MYTEST(function_parameter_negative_2)
{
    try
    {
        ts.analyze("int foo(int){return 1;}\n"
                   "int main(){foo(1);}");
        ASSERT_FALSE(true) << "function parameter needs a name";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 9)) << "Expected error at 1:9 - " << e.what();
    }
}

MYTEST(field_0)
{
    try
    {
        ts.analyze("struct Movie { int number; char name[50]; };\n"
                 "int main(){\n"
                     "\tstruct Movie movie;\n"
                     "\tmovie.number;\n"
                     "\tmovie.name;\n"
                     "\treturn 0;\n"
                 "}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(field_1)
{
    try
    {
      ts.analyze("struct Movie { int number; char name[50]; };\n"
               "int main(){\n"
                   "\tstruct Movie movie;\n"
                   "\tstruct Movie *ref;\n"
                   "\tref = &movie;\n"
                   "\tref->number;\n"
                   "\tref->name;\n"
                   "\treturn 0;\n"
               "}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(field_negative_0)
{
    try
    {
        ts.analyze("struct Movie { int number; char name[50]; };\n"
                 "int main(){\n"
                     "\tstruct Movie movie;\n"
                     "\tmovie.num;\n"
                     "\tmovie.name;\n"
                     "\treturn 0;\n"
                 "}");
        ASSERT_FALSE(true) << "Not defiened field name";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 4, 8)) << "Expected error at 4:8 - " << e.what();
    }
}


MYTEST(initialize_field_negative_1)
{
    try
    {
      ts.analyze("struct Movie { int number; char name[50]; };\n"
               "int main(){\n"
                   "\tstruct Movie movie;\n"
                   "\tstruct Movie *ref;\n"
                   "\tref = &movie;\n"
                   "\tmovie->num;\n"
                   "\tmovie->name;\n"
                   "\treturn 0;\n"
               "}");
        ASSERT_FALSE(true) << "Field name not defiened";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 6, 9)) << "Expected error at 6:9 - " << e.what();
    }
}

MYTEST(hiding_function_0)
{
    try
    {
        ts.analyze(
R"(
    int i(int a)
    {
        return a;
    }
    int main(void)
    {
        int i;
        i = 5;
        return i;
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

MYTEST(hiding_function_1)
{
    try
    {
        ts.analyze(
R"(
    int i (int a)
    {
        return a;
    }
    int main(void)
    {
        i(1);
        {
            int i;
            i = 5;
            i;
        }
        i(2);
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

MYTEST(hiding_function_2)
{
    try
    {
        ts.analyze(
R"(
    int i(int i)
    {
        return i;
    }
    int main(void)
    {
        i(1);
        return 0;
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

MYTEST(hiding_function_negative_0)
{
    try
    {
        ts.analyze(
R"(
    int i(int a)
    {
        return a;
    }
    int main()
    {
        i(1);
        int i;
        i = i(1);
    }
)"        
        );
        ASSERT_FALSE(true) << "function i is overshadowed by variable i";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 9, 13)) << "Expected error at 9:13 - " << e.what();
    }
}

MYTEST(hiding_function_negative_1)
{
    try
    {
        ts.analyze(
R"(
    int i(int a)
    {
        return a;
    }
    int b(int i)
    {
        return i(i);
    }
    int main()
    {
        b(1);
        return 0;
    }
)"
        );
        ASSERT_FALSE(true) << "parameter i is overshadowing the function i";
    }
    catch(std::exception& e)
    {
        // Currently error location is not as expected
        ASSERT_TRUE(ts.expect_err_loc(e, 8, 16)) << "Expected error at 9:13 - " << e.what();
    }
}

MYTEST(hiding_function_negative_2)
{
    try
    {
        ts.analyze(
R"(
    int i(int i){
        i(i);
    }
    int main(){
        i(1);
    }
)"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 9)) << "Expected error at 2:9 - " << e.what();
    }
}


MYTEST(program_scope_negative)
{
    try
    {
        ts.analyze(
R"(
    int i;
    int i(int a){return 1;}
    int main(){return 0;}
)"
        );
        ASSERT_FALSE(true) << "double declaration";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 3, 9)) << "Expected error at 3:9 - " << e.what();
    }
}

//6.2.3
MYTEST(scope_category_0)
{
    try
    {
        ts.analyze_body(
R"(
    int a;
    a: a;
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(scope_category_1)
{
    try
    {
        ts.analyze(
R"(
    struct a {};
    int a;
    int main(){return 0;};
)"        
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(scope_category_2)
{
    try
    {
        ts.analyze(
R"(
    struct a {int a;};
    int main(){return 0;}
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(scope_category_3)
{
    try
    {
        ts.analyze(
R"(
    struct a {int a;};
    int main()
    {
        struct a a;
        a: a.a;
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


