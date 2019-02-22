#include <types/ptr.hpp>
#include <types/function.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>

#include <llvm/IR/DerivedTypes.h>

#include <ir/irconstructer.hpp>

#include <iostream>

#include <util>

namespace mhc4
{

namespace types
{

ptr::ptr(type::ptr t)
    : t(std::move(t))
{  }

ptr::~ptr()
{  }

bool ptr::operator==(const type& typ) const
{
    auto* r = (typ.is_thunk()
               ?
               dynamic_cast<const types::ptr*>(static_cast<const thunk*>(&typ)->get().get())
               :
               dynamic_cast<const types::ptr*>(&typ));
    if(!r || typeid(*r) != typeid(*this))
    {
        // handle functions differently
        if(typ.is_function())
        {
            return *t.get() == typ;
        }
        return false;
    }
    const ptr& p = *util::thunk_cast<const ptr*>(&typ);

    return pointer_count() == p.pointer_count()
        && *t.get() == *p.t.get();
}

type* ptr::get_any_unprepared_thunk() noexcept
{
    return t->get_any_unprepared_thunk();
}

void ptr::print(std::size_t depth) const
{
    base_type()->print(depth);

    std::cout << " ";
    for(std::size_t i = 0; i < pointer_count(); ++i)
        std::cout << "(*";
    for(std::size_t i = 0; i < pointer_count(); ++i)
        std::cout << ")";
}

void ptr::print(std::string name, std::size_t depth) const
{
    auto* base = base_type();

    if(base->is_integral() || base->is_struct())
    {
        base_type()->print(depth);
        std::cout << " ";

        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << "(*";
        std::cout << name;
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << ")";
    }
    else if(base->is_function())
    {
        const types::function* f = util::thunk_cast<const types::function*>(base);
        type* ret = f->get_return_type().get();

        if(ret->is_integral() || ret->is_struct())
        {
            f->print_return_type(depth);
            std::cout << " ";
        }
        else if(ret->is_pointer())
        {
            const types::ptr* p = util::thunk_cast<const types::ptr*>(ret);
            p->base_type()->print(depth);
            std::cout << " ";
            for(std::size_t i = p->pointer_count(); i > 0; --i)
                std::cout << "(*";
        }
        else if(ret->is_array())
        {
            //probably most complex, as array must appear behind parameter
            std::cout << "TODO[types/ptr.cpp:76]TODO";
        }

        std::cout << "(";
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << "(*";
        std::cout << name;
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << ")";
        f->print_parameters(depth);
        std::cout << ")";

        if(ret->is_pointer())
        {
            const types::ptr* p = util::thunk_cast<const types::ptr*>(ret);
            for(std::size_t i = p->pointer_count(); i > 0; --i)
                std::cout << ")";
        }
    }
    else if(base->is_array())
    {
        const types::array* a = util::thunk_cast<const types::array*>(base);
        a->print_base(depth);
        std::cout << " (";
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << "(*";
        std::cout << name;
        a->print_array_part();
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << ")";
        std::cout << ")";
    }
    else
    {
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << "(*";
        std::cout << name;
        for(std::size_t i = pointer_count(); i > 0; --i)
            std::cout << ")";
    }
}

void ptr::infer(semantics::scope& s)
{
    // TODO: ?
    t->infer(s);
}

llvm::Type* ptr::generate_ir(irconstructer& irc, IRScope& irscope)
{
    if(t->is_void())
        return llvm::PointerType::getUnqual(irc._builder.getInt8Ty());
    return llvm::PointerType::getUnqual(t->generate_ir(irc, irscope));
}

bool ptr::is_void() const noexcept
{
    return false;
}

bool ptr::is_pointer() const noexcept
{
   return true;
}

bool ptr::is_integral() const noexcept
{
   return false;
}

bool ptr::is_function() const noexcept
{
    return false;
}

bool ptr::is_array() const noexcept
{
    return false;
}

bool ptr::is_struct() const noexcept
{
    return false;
}

bool ptr::is_thunk() const noexcept
{
    return false;
}

std::size_t ptr::pointer_count() const
{
    std::size_t val = 1U;
    if(t->is_pointer())
    {
        const types::ptr* p = util::thunk_cast<const types::ptr*>(t.get());
        val += p->pointer_count();
    }
    return val;
}

type* ptr::base_type() const
{
    if(t->is_pointer())
    {
        const types::ptr* p = util::thunk_cast<const types::ptr*>(t.get());
        return p->base_type();
    }
    return t.get();
}

type::ptr& ptr::dereference()
{
    return t;
}

const type::ptr& ptr::dereference() const
{
    return t;
}

type::ptr ptr::copy() const
{
    auto p = ::util::make_unique<types::ptr>(t->copy());
    p->set_location(this->location);
    return std::move(p);
}

}

}
