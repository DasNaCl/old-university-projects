#ifndef __WE_TEST_NOW_WITH_GOOGLETEST__

#include "include/cmd.hpp"
#include "include/application.hpp"

#include <iostream>

int main(int argc, char** const argv)
{
    mhc4::cmd cmd("c4", "Compiler Construction C Compiler by Henrik and Matthis");
    int to_return = EXIT_FAILURE;
    try
    {
        mhc4::cmd_info info = cmd.parse(argc, argv);
        mhc4::application app(info);

        to_return = app.compile();
    }
    catch(std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;

        return EXIT_FAILURE;
    }
    return to_return;
}

#endif
