#include <types/type.hpp>

#include <types/ptr.hpp>

namespace mhc4
{

namespace types
{

bool is_type(tok::token_kind kind) noexcept
{
    switch(kind)
    {
    case tok::keyword_char:
    case tok::keyword_void:
    case tok::keyword_int:
    case tok::keyword_struct:
        return true;
    default:
        return false;
    }
}

}

type::type()
    : location("", -1, -1)
{  }

type::~type()
{  }

void type::set_location(const source_location& loc)
{
    location = loc;
}

const source_location& type::get_location() const
{
    return location;
}

bool type::is_void_ptr() const noexcept
{
    if(!is_pointer())
        return false;
    auto* ptr = dynamic_cast<const types::ptr*>(this);
    assert(ptr);

    return ptr->dereference()->is_void();
}

bool type::is_aggregate() const noexcept
{
    return is_struct() || is_array();
}

bool type::is_arithmetic() const noexcept
{
    return !is_void() && !is_pointer() && !is_function()
        && !is_array() && !is_struct() && !is_thunk();
}

bool type::is_scalar() const noexcept
{
    return is_arithmetic() || is_pointer();
}

}
