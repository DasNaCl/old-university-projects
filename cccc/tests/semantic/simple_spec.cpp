#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(semantic_simple, sem_simple_ ## Y)

using namespace mhc4;

static const std::string f = "sem_simple.c";
const test_suite ts(f);

std::ostream& operator<<(std::ostream& os, const token& tok)
{
    return os << tok.to_string();
}



MYTEST(function_parameter_negative_0)
{
    try
    {
        ts.analyze("int foo(int a) {return a;}\n"
                   "int main(){foo(); return 0;}");
        ASSERT_FALSE(true) << "foo is missing a parameter!";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 12)) << "error loc should be 2:12 - " << e.what();
    }

}

MYTEST(function_parameter_negative_1)
{
    try
    {
        ts.analyze("int foo(int a) {return a;}\n"
                   "int main(){foo(1, 2); return 0;}");
        ASSERT_FALSE(true) << "foo is given too many parameters!";
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 2, 12)) << "error loc should be 2:12 - " << e.what();
    }
}

MYTEST(duplicate_struct)
{
    try
    {
        ts.analyze(
R"(
struct X
{
	int y;
};

struct X
{
	int z;
};
)");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 7, 8)) << "Error location should be 7:8 - " << e.what();
    }
}

MYTEST(duplicate_field)
{
    try
    {
        ts.analyze(
R"(
struct X
{
    int x;
};

struct Y
{
    int x;
};
)");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(duplicate_field_negative)
{
    try
    {
        ts.analyze(
R"(
struct X
{
	int y;
	char x;
	int y;
};
)");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 6, 6)) << "Error location should be 6:6 - " << e.what();
    }
}

MYTEST(duplicate_function_parameter)
{
    try
    {
        ts.analyze("int foo(int a){}\n"
                   "int bar(int a){}\n"
                   "int main(){foo(1); bar(2);}");
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(duplicate_function_parameter_negative)
{
    try
    {
        ts.analyze("int foo(int a, int a){}\n"
                   "int main(){foo(1, 2);}");
        ASSERT_FALSE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 1, 20)) << "Error location should be 1:20 - " << e.what();;
    }
}

MYTEST(same_param_var)
{
    try
    {
        ts.analyze(
R"(
void m(int f)
{
	int f;
}
)");
    }
    catch(std::exception& e)
    {
        ASSERT_TRUE(ts.expect_err_loc(e, 4, 6)) << "Error location should be 4:6 - " << e.what();
    }
}

MYTEST(semantic_digraph_0)
{
    try
    {
        ts.analyze(
R"(
    void foo() <%
        int a;
        <%
            a;
        %>
        return;
    %>

    int main() {
        foo();
    %>
)"
        );
        ASSERT_TRUE(true);
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(semantic_digraph_1)
{
    try
    {
        ts.analyze(
R"(
    void foo() {
        char* a;
        a[1];
        a<:1:>;
        a<:1];
        a[1:>;
    }

    int main() <%
        foo();
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





