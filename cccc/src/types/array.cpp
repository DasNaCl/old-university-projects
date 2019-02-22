#include <types/array.hpp>

#include <types/thunk.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace types
{

array::array(type::ptr of, std::size_t size)
    : of(std::move(of)), size(size)
{  }

array::~array()
{  }

bool array::operator==(const type& typ) const
{
    auto* r = (typ.is_thunk()
               ?
               dynamic_cast<const types::array*>(static_cast<const thunk*>(&typ)->get().get())
               :
               dynamic_cast<const types::array*>(&typ));
    if(!r || typeid(*r) != typeid(*this))
        return false;

    // you are an array
    const array& ar = *util::thunk_cast<const array*>(&typ);

    assert(of && ar.of);
    return size == ar.size && *of == *ar.of;
}

type* array::get_any_unprepared_thunk() noexcept
{
    return of->get_any_unprepared_thunk();
}

type::ptr array::base() const
{
    return of->copy();
}

void array::print(std::size_t depth) const
{
    of->print(depth);
    std::cout << "[";
    if(size != static_cast<std::size_t>(-1))
        std::cout << size;
    std::cout << "]";
}

void array::print(std::string name, std::size_t depth) const
{
    of->print(name, depth);
    std::cout << "[";
    if(size != static_cast<std::size_t>(-1))
        std::cout << size;
    std::cout << "]";
}

void array::print_base(std::size_t depth) const
{
    of->print(depth);
}

void array::print_array_part() const
{
    std::cout << "[";
    if(size != static_cast<std::size_t>(-1))
        std::cout << size;
    std::cout << "]";
}

void array::infer(semantics::scope& s)
{
    // TODO ?
    //  - maybe check if size is reasonable
    //  - maybe check if of is not void

    of->infer(s);
}

bool array::is_void() const noexcept
{
    return false;
}

bool array::is_pointer() const noexcept
{
    return false;
}

bool array::is_integral() const noexcept
{
    return false;
}

bool array::is_function() const noexcept
{
    return false;
}

bool array::is_array() const noexcept
{
    return true;
}

bool array::is_struct() const noexcept
{
    return false;
}

bool array::is_thunk() const noexcept
{
    return false;
}

type::ptr array::copy() const
{
    auto p = ::util::make_unique<array>(of->copy(), size);

    p->set_location(this->location);
    return std::move(p);
}

}

}
