#include "include/cmd.hpp"

#include <algorithm>

namespace mhc4
{

cmd::cmd(const std::string& program_name,
         const std::string& program_desc) noexcept
    : name(program_name), desc(program_desc), info(),
      args(), fileparser([this](int i, int, char** const argv)
                         {
                             // simply overwrite the single file we compile
                             info.file = argv[i];
                         })
{
    args["--tokenize"] = [this](int, int, char** const)
    { info.tokenize = true; info.compile = false; };

    args["--parse"] = [this](int, int, char** const)
    { info.parse = true; info.compile = false;};

    args["--print-ast"] = [this](int, int, char** const)
    { info.parse = info.print_ast = true; info.compile = false; };

    args["--compile"] = [this](int, int, char** const)
    { info.compile = true; };

    args["--optimize"] = [this](int, int, char** const)
    { info.optimize = true; };
}

const cmd_info& cmd::parse(int argc, char** const argv)
{
    // no arguments given
    if(argc < 2)
        return info; // <-- this is default at first

    for(int i = 0U; i < argc - 1; ++i)
    {
        std::string arg = argv[i + 1];

        if(args.find(arg) != args.end())
            args[arg](i + 1, argc, argv);
        else
            fileparser(i + 1, argc, argv);
    }

    return info;
}

const cmd_info& cmd::get_info() const
{
    return info;
}

}
