#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

namespace mhc4
{

struct cmd_info
{
    bool tokenize {false};
    bool parse {false};
    bool print_ast {false};
    bool compile {true};
    bool optimize {false};
    std::string file;
};

class cmd
{
public:
    cmd(const std::string& program_name,
        const std::string& program_desc) noexcept;

    const cmd_info& parse(int argc, char** const argv);

    const cmd_info& get_info() const;
private:
    std::string name;
    std::string desc;

    cmd_info info;

    using ParserType = std::function<void(int,int,char** const)>;
    std::map<std::string, ParserType> args;
    ParserType fileparser;
};

}
