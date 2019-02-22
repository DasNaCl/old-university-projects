#include <types/thunk.hpp>

#include <cassert>

namespace mhc4
{

namespace types
{

thunk::thunk()
    : t()
{  }

thunk::~thunk()
{  }

bool thunk::is_prepared() const noexcept
{
    return t.get();
}

void thunk::prepare(type::ptr d)
{
    t = std::move(d);
}

const type::ptr& thunk::get() const noexcept
{
    return t;
}

bool thunk::operator==(const type& typ) const
{
    // thunks are not really comparable....
    // fall back one level deeper
    return typ == *t.get();
}

type* thunk::get_any_unprepared_thunk() noexcept
{
    if(!is_prepared())
        return this;
    return t->get_any_unprepared_thunk();
}

void thunk::print(std::size_t d) const
{
    test();
    t->print(d);
}

void thunk::print(std::string name, std::size_t depth) const
{
    test();
    t->print(name, depth);
}

void thunk::infer(semantics::scope& s)
{
    test();
    t->infer(s);
}

llvm::Type* thunk::generate_ir(irconstructer& irc, IRScope& irscope)
{
    test();
    return t->generate_ir(irc, irscope);
}

bool thunk::is_void() const noexcept
{
    test();
    return t->is_void();
}

bool thunk::is_pointer() const noexcept
{
    test();
    return t->is_pointer();
}

bool thunk::is_integral() const noexcept
{
    test();
    return t->is_integral();
}

bool thunk::is_function() const noexcept
{
    test();
    return t->is_function();
}

bool thunk::is_array() const noexcept
{
    test();
    return t->is_array();
}

bool thunk::is_struct() const noexcept
{
    test();
    return t->is_struct();
}

bool thunk::is_thunk() const noexcept
{
    return true;
}

void thunk::test() const
{
    assert(t);
}

type::ptr thunk::copy() const
{
    return t->copy();
}

}

}
