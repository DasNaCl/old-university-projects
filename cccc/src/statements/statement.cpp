#include <statements/statement.hpp>

#include <iostream>

namespace mhc4
{

statement::statement(const source_location& loc) : loc(loc)
{  }

statement::~statement() noexcept
{  }

bool statement::is_if() const noexcept
{
    return false;
}

bool statement::is_block() const noexcept
{
    return false;
}

void statement::print_statement(std::size_t depth) const
{
    for(std::size_t d = depth; d > 0; --d)
        std::cout << "\t";
    print(depth);
    // we need this function in order to add ; for some children
}

}
