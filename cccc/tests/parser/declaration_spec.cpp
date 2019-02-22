#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(parser_declarations, declaration_ ## Y)

using namespace mhc4;

static const std::string f = "declare.c";
const test_suite ts(f);


MYTEST(identifier_int)
{
    try
    {
        ts.parse_body("int a;", 6);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(identifier_char)
{
    try
    {
        ts.parse_body("char a;", 7);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(identifier_void_negative)
{
    try
    {
        ts.parse_body("void a;", 7);
        ASSERT_FALSE(true) << "Variables are not allowed to be void";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(type_int)
{
    try
    {
        ts.parse_body("int;", 4);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(type_char)
{
    try
    {
        ts.parse_body("char;", 5);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_decl_void_0)
{
    try
    {
        ts.parse("void f();");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_decl_void_1)
{
    try
    {
        ts.parse("void f(int);");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_decl_void_2)
{
    try
    {
        ts.parse("void f(int asdasd);");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(complex_declarator_0)
{
    try
    {
        ts.parse_body("char (*(*(*(*(*(*(*x)))))));");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(complex_declarator_1)
{
    try
    {
        ts.parse_body("void** *** ** (** ** (*x));");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}


MYTEST(type_void)
{
    try
    {
      ts.parse_body("void;", 5);
    }
    catch(std::exception& e)
    {
      ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_nameless)
{
    try
    {
        ts.parse("int (int a) {return 0;} int main(){}");
        ASSERT_FALSE(true) << "Function needs a name";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(function_type_int)
{
    try
    {
        ts.parse("int foo(){} int main(){}", 24);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_type_char)
{
    try
    {
        ts.parse("char foo(){} int main(){}", 25);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_type_void)
{
    try
    {
        ts.parse("void foo(){} int main(){}", 25);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameter_0)
{
    try
    {
        ts.parse("void foo(int i){} int main(){}", 30);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameter_1)
{
    try
    {
        ts.parse("void foo(char c){} int main(){}", 31);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameter_2)
{
    try
    {
        ts.parse("void foo(void){} int main(){}", 29);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

// 6.7.6.3 6
MYTEST(function_parameter_3)
{
    try
    {
        ts.parse("void foo(int){} int main(){}", 28);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameter_negative_0)
{
    try
    {
        ts.parse("void foo(void v){} int main(){}", 31);
        ASSERT_FALSE("true") << "It is not allowed to have a void variable";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(function_parameter_multiple_0)
{
    try
    {
        ts.parse("void foo(int a, int b){} int main(){}", 37);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}


MYTEST(function_parameter_multiple_1)
{
    try
    {
        ts.parse("void foo(int a, char b){} int main(){}", 38);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(function_parameter_multiple_2)
{
    try
    {
        ts.parse("void foo(int a, char b, int c, int d){} int main(){}", 52);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

//NOTE might be semantic test
MYTEST(function_parameter_multiple_negative_0)
{
    try
    {
        ts.parse("void foo(void, void){} int main(){}", 52);
        ASSERT_FALSE(true) << "If void is in the parameter list, no additional"
                              "parameters are allowed";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

//NOTE might be semantic test
MYTEST(function_parameter_multiple_negative_1)
{
    try
    {
        ts.parse("void foo(void, int c){} int main(){}", 52);
        ASSERT_FALSE(true) << "If void is in the parameter list, no additional"
                              "parameters are allowed";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_0)
{
    try
    {
        ts.parse_body("struct wall;");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}


MYTEST(struct_1)
{
    try
    {
        ts.parse_body("struct wall {int;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_2)
{
    try
    {
        ts.parse_body("struct wall {int a;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_3)
{
    try
    {
        ts.parse_body("struct wall {int a; char b; void;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_4)
{
    try
    {
        ts.parse_body("struct wall; struct mall {struct wall;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_5)
{
    try
    {
        ts.parse_body("struct {int;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_6)
{
    try
    {
        ts.parse_body("struct {struct wall;};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_7)
{
    try
    {
        ts.parse_body("struct {struct wall {int a; char b;};};");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_negative_0)
{
    try
    {
        ts.parse_body("struct wall {int a;}");
        ASSERT_FALSE(true) << "struct declarations require ; at the end";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_1)
{
    try
    {
        ts.parse_body("struct wall {int a};");
        ASSERT_FALSE(true) << "struct declarations require ; within after type declaration";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_2)
{
    try
    {
        ts.parse_body("struct;");
        ASSERT_FALSE(true) << "struct needs either an identifier or a body with declarations";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_3)
{
    try
    {
        ts.parse_body("struct {};");
        ASSERT_FALSE(true) << "struct body is not allowed to be empty";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_4)
{
    try
    {
        ts.parse_body("struct wall {};");
        ASSERT_FALSE(true) << "struct body is not allowed to be empty";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_5)
{
    try
    {
        ts.parse_body("struct wall {void fun();};");
        ASSERT_FALSE(true) << "function declarator is not allowed in struct";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_6)
{
    try
    {
        ts.parse_body("struct wall {int fun();};");
        ASSERT_FALSE(true) << "function declarator is not allowed in struct";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_7)
{
    try
    {
        ts.parse_body("struct wall {char fun();};");
        ASSERT_FALSE(true) << "function declarator is not allowed in struct";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(struct_negative_8)
{
    try
    {
        ts.parse_body("struct wall {int* fun();};");
        ASSERT_FALSE(true) << "function declarator is not allowed in struct";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(array_0)
{
    try
    {
        // array of pointers to int values
        ts.parse_body("int *var[5];");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(array_1)
{
    try
    {
        // pointer to array of int values
        ts.parse_body("int (*var)[5];");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_0)
{
    try
    {
        ts.parse_body("char *(*(*var)())[10];");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_1)
{
    try
    {
        // function returning pointer to int
        ts.parse_body("int *var(int, int);");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_2)
{
    try
    {
        // pointer to function returning int
        ts.parse_body("int (*var)(int, int);");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_3)
{
    try
    {
        // function taking int and returning function pointer
        ts.parse_body("int (*f(int n))(int, int);");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_4)
{
    try
    {
        ts.parse_body("int *(**name[5][10])(void);");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(ptrfunc_5)
{
    try
    {
        ts.parse("int (*p)(int (*));");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(ptrfunc_6)
{
    // compare to ast test declarators
    try
    {
        ts.parse("int (*(*(*(*p(void))(int (*)))[5])(void));");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

MYTEST(struct_as_ret)
{
    try
    {
        // array of pointer to functions returning structures
        ts.parse_body(
R"(
struct test
{
    int a;
    int b;
    char c;
} (*var[5])(struct test, struct test); 
)");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(struct_ptr_array_0)
{
    try
    {
        // array of pointer to functions returning structures
        ts.parse_body(
R"(
struct test
{
    int a;
    char c;
} **var[5][42]; 
)");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(struct_ptr_array_1)
{
    try
    {
        // array of pointers to arrays of pointers to structs 
        ts.parse_body("struct test *(*var[5])[5];");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(directly_assign)
{
    try
    {
        ts.parse("int a = 0;");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(false);
    }
}

MYTEST(non_block)
{
    try
    {
        ts.parse("int a[100];");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true);
    }
}

/*
MYTEST(non_block_negative_0)
{
    try
    {
        ts.parse("int a;\nint b[a];");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}
*/

MYTEST(non_block_negative_0)
{
    try
    {
        ts.parse_body(
R"(
    if (1) int x;
)"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}

MYTEST(non_block_negative_1)
{
    try
    {
        ts.parse_body(
R"(
    if (1) 1;
    else int x;
)"
        );
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(true);
    }
}
