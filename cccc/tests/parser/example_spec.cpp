#include <test_suite.hpp>

#include <gtest/gtest.h>

#define MYTEST(Y) TEST(parser_examples, example_ ## Y)

using namespace mhc4;

static const std::string f = "example.c";
const test_suite ts(f);

//TODO check if tests are correct
MYTEST(faculty)
{
    try
    {
        ts.parse(
          "int fac(int i){"
              "int a;"
              "a = 1;"
              "while (i > 0) {"
                    "a = a * i;"
                    "i = i - 1;"
              "}"
              "return a;"
          "}"
          "int main() { fac(5); }");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(faculty_recursive)
{
    try
    {
        ts.parse(
          "int fac (int i){"
              "if (i < 2) return 1;"
              "else return i * fac(i - 1);"
          "}"
          "int main() { fac(5); }");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(fibonacci)
{
    try
    {
        ts.parse(
          "int fib(int nth){"
              "int a; a = 1;"
              "int b; b = 1;"
              "while(nth > 1){"
                  "int tmp; tmp = a;"
                  "a = a + b;"
                  "b = tmp;"
                  "nth = nth - 1;"
              "}"
              "return a;"
          "}"
          "int main() { fib(5); }");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(fibonacci_array)
{
    try
    {
        ts.parse(
R"(
void* malloc(int size);
int fib(int i)
{
    if(i < 2)
        return i;
    int* arr = /*(int*)*/malloc(sizeof(int) * i);
    arr[0] = 0;
    arr[1] = 1;

    int j = 2;
    while (j <= i)
    {
        arr[j] = arr[j - 1] + arr[j - 2];
        j = j + 1;
    }
    return arr[i];
}

int main()
{
    return fib(13);
}
)");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(fibonacci_recursive_0)
{
    try
    {
        ts.parse(
          "int fib(int nth){"
              "if (nth < 2)"
                  "return 1;"
              "else "
                  "return fib(nth - 1) + fib(nth - 2);"
          "}"
          "int main() { fib(5); }");
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(fibbonacci_recursive_1)
{
    try
    {
        ts.parse(
R"(
    int fib(int n)
    {
        return (n < 2) ? 1 : fib(n - 1) + fib(n - 2);
    }
    int main(void){
        fib(10);
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

MYTEST(fibonnaci_bad_array)
{
    try
    {
        ts.parse(
R"(
    void* malloc(int size);
    void free(void* data);
    int fib(int n) {
        int* arr = malloc(sizeof int * n);
        int result;
        if(n < 2){
            result = n;
        }
        else {
            arr[0] = 1;
            arr[1] = 1;
            int i = n;
            while(i <= n) {
                arr[i] = fib(i - 1) + fib(i - 2);
            }
            result = arr[n];
        }
        free(arr);
        return result;
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

MYTEST(fibonacci_with_declarator)
{
    try
    {
        ts.parse(
R"(
    int fib(int n);
    int main(){
        fib(10);
    }
    int fib(int n){
        return (n < 2) ? n : fib(n-1) + fib(n-2);
    }
)"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(struct_manipulation) 
{
    try
    {
        ts.parse(
R"(
    struct image {
        int r;
        int g;
        int b;
        int* data;
    };

    int main(void){
        struct image img;
        img.r = 10;
        struct image* iref;
        iref->g = 15;
        (*iref).b = 20;
        (iref->data)[0] = 100;
    }
)"
        );
    }
    catch(std::exception& e)
    {
        ASSERT_FALSE(true) << e.what();
    }
}

MYTEST(hello_world)
{
    try
    {
        ts.parse(
R"(
    void print(char* arg){/*I print stuff*/}

    int main(int argc, char** argv) {
        print("Hello World!");
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
