#pragma once

#include <cmd.hpp>

namespace mhc4
{

class application
{
public:
    explicit application(const cmd_info& info) noexcept;

    int compile();
private:
    const cmd_info& info;
};

}
