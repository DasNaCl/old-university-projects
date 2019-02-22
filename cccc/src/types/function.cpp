#include <types/function.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace types
{

function::function(type::ptr return_type,
                   std::vector<type::ptr> param_types)
    : return_type(std::move(return_type)), param_types(std::move(param_types))
{
    if(param_types.size() == 1)
    {
        if(param_types[0]->is_void())
            param_types.clear();
    }
}

function::~function()
{  }

bool function::operator==(const type& typ) const
{
    auto* r = (typ.is_thunk()
               ?
               dynamic_cast<const types::function*>(static_cast<const thunk*>(&typ)->get().get())
               :
               dynamic_cast<const types::function*>(&typ));
    if(!r || typeid(*r) != typeid(*this))
    {
        // try to find function pointers
        if(typ.is_pointer())
        {
            auto* ptr = dynamic_cast<const types::ptr*>(&typ);
            auto* der = ptr->dereference().get();

            return *this == *der;
        }
        return false;
    }
    const function& func = *util::thunk_cast<const function*>(&typ);

    bool differ = param_types.size() != func.param_types.size();
    for(std::size_t i = 0; !differ && i < std::min(param_types.size(), func.param_types.size()); ++i)
    {
        const auto& a = *param_types[i].get();
        const auto& b = *func.param_types[i].get();

        if(!(a == b))
            differ = true;
    }

    return *return_type.get() == *func.return_type.get() && (!differ || param_types.empty() || func.param_types.empty());
}

type* function::get_any_unprepared_thunk() noexcept
{
    // parameters do not have any unprepared thunks
    return return_type->get_any_unprepared_thunk();
}

void function::print(std::size_t depth) const
{
    print("", {}, depth);
}

void function::print(std::string name, std::size_t depth) const
{
    print(name, {}, depth);
}

void function::print(std::string name, std::vector<std::string> args, std::size_t depth) const
{
    if(return_type->is_integral() || return_type->is_struct())
    {
        return_type->print(depth);
        std::cout << " (" << name << "(";

        if(!param_types.empty())
        {
            for(std::size_t i = 0; i < param_types.size(); ++i)
            {
                if(!args.empty())
                {
                    assert(args.size() == param_types.size());
                    print_parameter_type(i, args[i], depth);
                }
                else
                    print_parameter_type(i, depth);
                if(i + 1 < param_types.size())
                    std::cout << ", ";
            }
        }
        std::cout << "))";
    }
    else if(return_type->is_array())
    {
        // arrays are not supported
        assert(false && "not implemented!");
    }
    else if(return_type->is_pointer())
    {
        const types::ptr* p = util::thunk_cast<const types::ptr*>(return_type.get());
        if(p->base_type()->is_array())
        {
            std::cout << "TODO[types/function.cpp:86]TODO";
        }
        else if(p->base_type()->is_function())
        {
            // TODO: this needs to be done recursively, somehow
            //    Note: "this" means printing a functions return type
            //          before, then id and other stuff in between
            //          and, lastly, it's parameters recursively
            std::cout << "TODO[types/function.cpp:92]TODO";
        }
        else
        {
            p->base_type()->print(depth);
        }
        std::cout << " ";
        for(std::size_t i = 0U; i < p->pointer_count(); ++i)
            std::cout << "(*";
        std::cout << "(" << name;

        std::cout << "(";
        if(!param_types.empty())
        {
            for(std::size_t i = 0; i < param_types.size(); ++i)
            {
                if(!args.empty())
                {
                    assert(args.size() == param_types.size());
                    print_parameter_type(i, args[i], depth);
                }
                else
                    print_parameter_type(i, depth);
                if(i + 1 < param_types.size())
                    std::cout << ", ";
            }
        }
        else
            std::cout << "void";
        std::cout << "))";

        for(std::size_t i = 0U; i < p->pointer_count(); ++i)
            std::cout << ")";
    }

}

void function::infer(semantics::scope& s)
{
    if(return_type->is_function() || return_type->is_array())
    {
        throw std::invalid_argument(return_type->is_function()
                                    ?
                                    "Function may not return another function!"
                                    :
                                    "Function may not return an array!");
    }

    return_type->infer(s);
    for(const auto& p : param_types)
        p->infer(s);
}

llvm::Type* function::generate_ir(irconstructer& irc, IRScope& irscope)
{
    llvm::Type* ret = return_type->generate_ir(irc, irscope);
    std::vector<llvm::Type*> parameter_types;

    if(param_types.size() > 0 && !param_types[0]->is_void())
    {
        for(auto& x : param_types)
            parameter_types.push_back(x->generate_ir(irc, irscope));
    }
    else
    {
        assert(param_types.size() == 0
           || (param_types.size() == 1 && param_types[0]->is_void()));
    }

    return llvm::FunctionType::get(ret, parameter_types, false);
}

const type::ptr& function::parameter_type(std::size_t index) const
{
    assert(index < param_types.size());

    return param_types[index];
}

const type::ptr& function::get_return_type() const
{
    return return_type;
}

std::size_t function::parameter_count() const
{
    return param_types.size();
}

void function::print_return_type(std::size_t depth) const
{
    return_type->print(depth);
}

void function::print_parameter_type(std::size_t index, std::size_t depth) const
{
    assert(index < param_types.size());

    param_types[index]->print(depth);
}

void function::print_parameter_type(std::size_t index, const std::string& name, std::size_t depth) const
{
    assert(index < param_types.size());

    param_types[index]->print(name, depth);
}

void function::print_parameters(std::size_t depth) const
{
    std::cout << "(";
    for(std::size_t i = 0U; i < param_types.size(); ++i)
    {
        print_parameter_type(i, depth);
        if(i + 1 < param_types.size())
            std::cout << ", ";
    }
    std::cout << ")";
}

bool function::returns_void() const noexcept
{
    return return_type->is_void();
}

bool function::is_void() const noexcept
{
    return false;
}

bool function::is_pointer() const noexcept
{
    return false;
}

bool function::is_integral() const noexcept
{
    return false;
}

bool function::is_function() const noexcept
{
    return true;
}

bool function::is_array() const noexcept
{
    return false;
}

bool function::is_struct() const noexcept
{
    return false;
}

bool function::is_thunk() const noexcept
{
    return false;
}

type::ptr function::copy() const
{
    std::vector<type::ptr> param_cpy;
    for(const auto& c : param_types)
        param_cpy.emplace_back(c->copy());
    auto p = ::util::make_unique<function>(return_type->copy(), std::move(param_cpy));

    p->set_location(this->get_location());
    return std::move(p);
}

}

}
