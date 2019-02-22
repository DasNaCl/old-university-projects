#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(semantic_type, type_ ## Y)

using namespace mhc4;

static const std::string f = "sem_type.c";
const test_suite ts(f);

MYTEST(simple_0)
{
    try
    {
        ts.analyze_body(
            "int a; int b; a+b;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(simple_1)
{
    try
    {
        ts.analyze_body(
            "char a; char b; a = b;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

// changed from simple_negative_0 due to integer promotion
MYTEST(simple_2)
{
    try
    {
        ts.analyze_body(
            "int a; char b; a = b;"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

// changed from simple_negative_1 due to integer promotion
MYTEST(simple_3)
{
    try
    {
        ts.analyze_body(
            "int a; char b; a + b;"
        );
        ASSERT_TRUE(true) << "Shall pass due integer promotion";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

//NOTE currently not sure whether pointer should behave
// as the tests currently check, will investigate further

MYTEST(pointer_0)
{
    try
    {
        ts.analyze_body(
            "int a; int *b;\n"
            "b = &a;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(pointer_1)
{
    try
    {
        ts.analyze_body(
            "char b[]; b = \"Hallo\";"
        );
    }
    catch(std::exception& e)
    { 
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(pointer_2)
{
    try
    {
        ts.analyze_body(
            "int a; int *b; *b = a;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(pointer_negative_0)
{
    try
    {
        ts.analyze_body(
            "int a; int *b; a = b;"
        );
        ASSERT_FALSE(true) << "type mismatch";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 18, true)) << "Expected err_loc 1:18 - " << e.what();
    }
}

MYTEST(pointer_negative_1)
{
    try
    {
        ts.analyze_body(
            "int a; char *b; b = &a;"
        );
        ASSERT_FALSE(true) << "type mismatch";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 19, true)) << "Expected err_loc 1:19 - " << e.what();
    }
}

MYTEST(compare_0)
{
    try
    {
        ts.analyze_body(
            "int a; int b; a == b;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compare_1)
{
    try
    {
        ts.analyze_body(
            "int *a; int *b; a == b;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compare_2)
{
    try
    {
        ts.analyze_body(
            "char a; char b; a == b;"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(compare_3)
{
    try
    {
        ts.analyze(
R"(
    int a() {return 1;}
    int b() {return 2;}
    int main()
    {
        int c;
        c = a() == b();
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


// NOTE 
// "Due to the restricted language subset, type compatibility degenerates to equality" -task e
// Because of the previous statement I believe the following tests are wrong, until no other
// conclusion will be drawn, the following tests are degraded to comments

/*
MYTEST(compare_negative_0)
{
    try
    {
        ts.analyze_body(
            "int a; char b; a == b;"
        );
        ASSERT_FALSE(true) << "type mismatch";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(compare_negative_1)
{
    try
    {
        ts.analyze_body(
            "int a; int *b; a == b;"
        );
        ASSERT_FALSE(true) << "type mismatch";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(compare_negative_2)
{
    try
    {
        ts.analyze_body(
            "char *a; int *b; a == b;"
        );
        ASSERT_FALSE(true) << "type mismatch";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}
*/

MYTEST(function_0)
{
    try
    {
        ts.analyze(
R"(
    int foo(){return 42;}
    int main()
    {
        int a;
        a = foo();
    }
)"
        );
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
        ts.analyze(
R"(
    int foo(){return 42;}
    char bar(int a) {return 'a';}
    int main()
    {
        char c;
        c = bar(foo());
    }
)"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

// changed from function_negative_0 due to integer promotion
MYTEST(function_2)
{
    try
    {
        ts.analyze(
R"(
    int foo(){return 42;}
    int main()
    {
        char c;
        c = foo();
    }
)"
        );
        ASSERT_TRUE(true) << "Shall pass due to integer promotion";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

// changed from function_negative_1 due to integer promotion
MYTEST(function_3)
{
    try
    {
        ts.analyze(
R"(
    int foo(){return 42;}
    int bar(char c){return 1;}
    int main()
    {
        int a;
        a = bar(foo());
    }
)"
        );
        ASSERT_TRUE(true) << "Shall pass due to integer promotion";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_return_0)
{
    try
    {
        ts.analyze(
R"(
    int foo() {char c; return c;}
    int main(){}
)"
        );
        ASSERT_TRUE(true) << "Shall pass due integer promotion";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_return_1)
{
    try
    {
        ts.analyze(
R"(
    int foo(){return 'a';}
    int main(){}
)"
        );
        ASSERT_TRUE(true) << "Shall pass due to integer promotion";
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(period_arrow)
{
   try
    {
      ts.analyze("struct Movie { int number; char name[50];};\n"
                   "int main(){\n"
                   "struct Movie movie;\n"
                   "struct Movie *ref;\n"
                   "ref = &movie;\n"
                   "movie.number;\n"
                   "movie.name;\n"
                   "ref->number;\n"
                   "ref->name;\n"
               "}");
      ASSERT_TRUE(true);
    } 
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(arrow_negative)
{
    try
    {
      ts.analyze("struct Movie { int number; char name[50];};\n"
                   "int main(){\n"
                   "struct Movie movie;\n"
                   "movie->number;\n"
                   "movie->name;\n"
               "}");
      ASSERT_FALSE(true) << "Type error.";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 4, 6)) << "Expected err_loc 4:6 - " << e.what();
    }
}

MYTEST(period_negative)
{
    try
    {
      ts.analyze("struct Movie { int number; char name[50];};\n"
              "int main(){\n"
                  "struct Movie movie;\n"
                  "struct Movie *ref;\n"
                  "ref = &movie;\n"
                  "ref.number;\n"
                  "ref.name;\n"
              "}");
      ASSERT_FALSE(true) << "Type error, applying period operator to pointer";
    }
    catch(std::exception& e)
    {
      ASSERT_TRUE(ts.expect_err_loc(e, 6, 4)) << "Expected err_loc 6:4 - " << e.what();
    }
}

MYTEST(conditional_negative)
{
    try
    {
        ts.analyze_body("1 ? 1 : \"hi\";");
        // here we have an type mismatch with an arithmetic type and a pointer type
        // which is not allowed in an conditional expression.
        ASSERT_FALSE(true) << "type mismatch, operator precendence error, see link in comment.";
        // https://stackoverflow.com/questions/13318336/what-is-causing-warning-pointer-integer-type-mismatch-in-conditional-expressio
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 3, true)) << "Expected err_loc 1:3 - " << e.what();
    }
}

MYTEST(l_value_negative_0)
{
    try
    {
        ts.analyze_body("&1;");
        ASSERT_FALSE(true) << "Expected to fail since l_value is required for unary \"&\"";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 1, true)) << "Expected err_loc 1:1 - " << e.what();
    }
}

MYTEST(l_value_negative_1)
{
    try
    {
        ts.analyze_body("int a; int b; int c; a || b = c;");
        ASSERT_FALSE(true) << "left side of \"=\" operator needs l-value";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 29, true)) << "Expected err_loc 1:29 - " << e.what();
    }
}

MYTEST(struct_0)
{
    try
    {
        ts.analyze(
R"(
    struct image {
        int r;
        int g;
        int b;
        int* data;
    };

    int main(){
        struct image img;
        img.r = 10;
        struct image* iref;
        iref->g = 15;
        (*iref).b = 20;
        (iref->data)[0] = 100;
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
