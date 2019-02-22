#include <sema/scope.hpp>
#include <sema/analyzer.hpp>

#include <statements/label.hpp>

#include <types/function.hpp>
#include <types/integral.hpp>
#include <types/struct.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <ir/IRScope.hpp>

#include <algorithm>
#include <cassert>
#include <deque>

namespace mhc4
{

namespace semantics
{

std::map<std::string, std::size_t> scope::struct_counter;

scope::scope(scope* parent)
    : parent(parent), data(), structs(), labels(), gotos(),
      hasloop(false), infunction(parent ? parent->infunction : nullptr)
{  }

scope scope::inherit()
{
    return scope(this);
}

scope scope::create()
{
    return scope(nullptr);
}

bool scope::is_external() const noexcept
{
    return !parent;
}

void scope::set_in_loop()
{
    hasloop = true;
}

bool scope::in_loop() const
{
    if(hasloop)
        return true;
    if(parent)
        return parent->in_loop();
    return false;
}

void scope::set_in_function(types::function& func)
{
    infunction = &func;
}

bool scope::in_function() const
{
    if(infunction)
        return true;
    if(parent)
        return parent->in_function();
    return false;
}

types::function* scope::get_function() const
{
    if(infunction)
         return infunction;
    if(parent)
        return parent->get_function();
    assert(false);
    return nullptr;
}

bool scope::has_any(const std::string& name) const
{
    for(const auto& pair : data)
    {
        const auto& set = pair.second;
        auto it = std::find_if(set.begin(), set.end(),
                               [&name](const decl& p)
                               { return name == p.name; });
        if(it != set.end())
            return true;
    }
    if(parent)
        return parent->has_any(name);
    return false;
}

bool scope::has_label(const std::string& name) const
{
    auto it = std::find(labels.begin(), labels.end(), name);
    if(it != labels.end())
        return true;
    if(parent && it == labels.end() && parent->infunction == infunction)
        return parent->has_label(name);
    return false;
}

class struct_decl
{  };

template<>
void scope::insert<struct_decl>(const token& name, const type::ptr& typ)
{
    bool any = false;
    for(const auto& pair : data)
    {
        const auto& set = pair.second;
        auto it = std::find_if(set.begin(), set.end(),
                               [&name](const decl& p)
                               { return name.userdata() == p.name; });
        if(it != set.end())
            any = true;
    }
    if(any)
    {
        already_declared_exception<types::_struct> ex(name.userdata());
        auto& loc = name.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    auto& set = data[std::type_index(typeid(struct_decl))];

    auto* ptr = find_struct_ptr(typ);
    // if we add a struct declaration, the definition must
    // exist, thus the assertion for ptr
    assert(ptr);
    set.push_back(decl{typ->copy(), name.userdata(), true, ptr, ""});
}

template<>
void scope::insert<types::ptr>(const token& name, const type::ptr& typ)
{
    bool any = false;
    for(const auto& pair : data)
    {
        const auto& set = pair.second;
        auto it = std::find_if(set.begin(), set.end(),
                               [&name](const decl& p)
                               { return name.userdata() == p.name; });
        if(it != set.end())
            any = true;
    }
    if(any)
    {
        already_declared_exception<types::_struct> ex(name.userdata());
        auto& loc = name.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    auto& set = data[std::type_index(typeid(types::ptr))];

    auto* conv = dynamic_cast<types::ptr*>(typ.get());
    assert(conv);
    auto* base = conv->base_type();
    type::ptr* ptr = nullptr;
    if(base->is_struct())
    {
        ptr = find_struct_ptr(base->copy());

        // if we add a struct* declaration, the definition shall
        // or shall not exist, thus no to make assertion here
        ;
    }


    set.push_back(decl{typ->copy(), name.userdata(), true, ptr, ""});
}

template<>
std::size_t scope::height_of_var<types::function>(const std::string& str) const
{
    auto it = std::find_if(functions.begin(), functions.end(),
                           [&str](const decl& p)
                           { return str == p.name; });
    if(it != functions.end())
        return 0;
    else if(parent)
    {
        const std::size_t res = parent->height_of_var<types::function>(str);
        return (res != static_cast<std::size_t>(-1) ? 1 + res : res);
    }

    // not found
    return -1;
}

scope::decl* scope::find_decl_for_var(const token& tok)
{
    std::size_t _struct   = height_of_var<struct_decl>(tok.userdata());
    std::size_t _ptr      = height_of_var<types::ptr>(tok.userdata());
    std::size_t _integral = height_of_var<types::_integral>(tok.userdata());
    std::size_t _array    = height_of_var<types::array>(tok.userdata());
    std::size_t _function = height_of_var<types::function>(tok.userdata());

    assert(!(_struct != static_cast<std::size_t>(-1))
           || (_struct != _ptr && _struct != _integral
            && _struct != _array && _struct != _function));
    assert(!(_ptr != static_cast<std::size_t>(-1))
           || (_ptr != _struct && _ptr != _integral
            && _ptr != _array && _ptr != _function));
    assert(!(_integral != static_cast<std::size_t>(-1))
           || (_integral != _struct && _integral != _ptr
            && _integral != _array && _integral != _function));
    assert(!(_array != static_cast<std::size_t>(-1))
           || (_array != _struct && _array != _integral
            && _array != _ptr && _array != _function));
    assert(!(_function != static_cast<std::size_t>(-1))
           || (_function != _struct && _function != _integral
            && _function != _ptr && _function != _array));

    if(_struct == _ptr && _ptr == _integral && _integral == _array
    && _array == _function && _function == static_cast<std::size_t>(-1))
    {
        auto& loc = tok.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: Identifier \"" + tok.userdata() + "\" not declared.");
    }

    return get_closest_decl(tok.userdata());
}

type::ptr scope::find_struct(const type* typ)
{
    assert(dynamic_cast<const types::_struct*>(typ));
    auto* strct = static_cast<const types::_struct*>(typ);

    assert(strct->has_name());
    token tok = strct->name();

    auto t = get_closest_struct_ptr("struct." + tok.userdata());
    if(t)
        return (*t)->copy();
    return type::ptr(nullptr);
}

void scope::insert_decl(const type::ptr& t, const token& id)
{
    auto* res = t.get();
    while(res->is_thunk())
        res = static_cast<const types::thunk*>(res)->get().get();

    if(dynamic_cast<types::_struct*>(res))
        insert<struct_decl>(id, res->copy());
    else if(dynamic_cast<types::ptr*>(res))
        insert<types::ptr>(id, res->copy());
    else if(dynamic_cast<types::_integral*>(res))
        insert<types::_integral>(id, res->copy());
    else if(dynamic_cast<types::array*>(res))
        insert<types::array>(id, res->copy());
    else if(dynamic_cast<types::function*>(res))
        insert<types::function>(id, res->copy());
    else
        assert(false);
}


type::ptr scope::var_struct_definition(const std::string& str) const
{
    for(auto& x : data)
    {
        for(auto& s : x.second)
        {
            if(str == s.name && s.defined)
            {
                assert(s.struct_definition);
                return (*s.struct_definition)->copy();
            }
        }
    }
    if(parent)
        return parent->var_struct_definition(str);
    return type::ptr(nullptr);
}

void scope::insert_struct(const token& name, const type::ptr& typ, bool defined)
{
    // struct definitions may be shadowed, that's why we only
    // look at the current scope and not in all parents as well
    auto res = std::find_if(structs.begin(), structs.end(),
                            [&name](const decl& p)
                            { return p.defined && p.name == "struct." + name.userdata(); });
    if(res != structs.end() && defined)
    {
        already_declared_exception<types::_struct> ex(name.userdata());
        auto& loc = name.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    else if(res != structs.end() && !defined)
        return;

    res = std::find_if(structs.begin(), structs.end(),
                       [&name](const decl& p)
                       { return !p.defined && p.name == "struct." + name.userdata(); });
    if(res != structs.end() && defined)
    {
        res->defined = true;
        res->typ = typ->copy();

        auto* strct = dynamic_cast<types::_struct*>(res->typ.get());
        assert(strct);
        //strct->set_irname(irname);
        update_struct_references(name.userdata(), &res->typ);
        return;
    }
    else if(res != structs.end() && !defined)
        return;

    auto tt = get_closest_decl("struct." + name.userdata());
    if(tt)
        structs.push_back(decl{tt->typ->copy(),
                               "struct." + name.userdata(),
                               true, nullptr, ""});
    else
        structs.emplace_back(decl{typ->copy(),
                                  "struct." + name.userdata(),
                                  defined, nullptr, ""});
    auto* strct = dynamic_cast<types::_struct*>(structs.back().typ.get());
    assert(strct);
    //strct->set_irname(irname);

    if(defined)
    {
        structs.back().struct_definition = &structs.back().typ;
    }
}

void scope::insert_function(const token& tok, const type::ptr& typ, bool defined)
{
    bool any = false;
    {
        auto it = std::find_if(functions.begin(), functions.end(),
                               [&tok](const decl& p)
                               { return tok.userdata() == p.name; });
        if(it != functions.end())
        {
            // we found a already declared/defined function !
            if(it->defined && defined)
            {
                // we are considered a duplicate
                any = true;
            }
            else
            {
                if(!(*it->typ == *typ))
                    any = true;
                else
                {
                    if(!it->defined && defined)
                        it->defined = defined;
                    return;
                }
            }
        }
    }
    if(any)
    {
        already_declared_exception<types::function> ex(tok.userdata());
        auto& loc = tok.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    functions.emplace_back(decl{typ->copy(), tok.userdata(),
                                defined, nullptr, ""});
}

void scope::insert_label(const token& name)
{
    //first, propagate to topmost scope
    if(parent && parent->infunction == infunction)
    {
        parent->insert_label(name);
        return;
    }

    if(has_label(name.userdata()))
    {
        already_declared_exception<statements::label> ex(name.userdata());
        auto& loc = name.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    labels.push_back(name.userdata());
}

void scope::insert_goto(const token& name)
{
    if(parent && parent->infunction == infunction)
        parent->insert_goto(name);

    gotos.push_back(name);
}

void scope::check_goto()
{
    for(auto& tok : gotos)
    {
        auto it = std::find_if(labels.begin(), labels.end(),
                               [tok](const std::string& str)
                               { return str == tok.userdata(); });
        if(it == labels.end())
        {
            semantics::throw_error(tok, "Missing label");
        }
    }
}

scope::decl* scope::get_closest_decl(const std::string& str)
{
    std::string struct_or_fun_name = str;
    // vars
    for(auto& p : data)
    {
        auto& vec = p.second;
        auto it = std::find_if(vec.begin(), vec.end(),
                               [&str](const decl& pair)
                               { return pair.name == str; });

        if(it != vec.end())
        {
            if(p.first == std::type_index(typeid(struct_decl)))
            {
                assert(it->struct_definition);
            }
            return &*it;
        }
    }

    // functions
    {
    auto it = std::find_if(functions.begin(), functions.end(),
                           [&struct_or_fun_name](const decl& tup)
                           { return struct_or_fun_name == tup.name; });
    if(it != functions.end())
        return &*it;
    }

    if(parent)
    {
        auto t = parent->get_closest_decl(struct_or_fun_name);
        if(t)
            return t;
    }

    // uh oh
    return nullptr;
}

type::ptr* scope::find_struct_ptr(const type::ptr& typ)
{
    assert(dynamic_cast<const types::_struct*>(typ.get()));
    auto* strct = static_cast<const types::_struct*>(typ.get());

    assert(strct->has_name());
    token tok = strct->name();
    return get_closest_struct_ptr("struct." + tok.userdata());
}

type::ptr* scope::get_closest_struct_ptr(const std::string& str)
{
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&str](const decl& tup)
                           { return tup.defined && str == tup.name; });
    if(it != structs.end())
        return &it->typ;
    }
    auto* p = (parent ? parent->get_closest_struct_ptr(str) : nullptr);
    if(p)
        return p;
    //fall back to not yet defined ones (this is the case for forward declarations)
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&str](const decl& tup)
                           { return !tup.defined && str == tup.name; });
    if(it != structs.end())
        return &it->typ;
    }

    // uh oh
    return nullptr;
}

scope::decl* scope::find_struct_as_decl(const type* typ)
{
    assert(typ);
    assert(dynamic_cast<const types::_struct*>(typ));
    auto* strct = static_cast<const types::_struct*>(typ);

    assert(strct->has_name());
    token tok = strct->name();
    return find_struct_as_decl_helper("struct." + tok.userdata());
}


scope::decl* scope::find_struct_as_decl_helper(const std::string& str)
{
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&str](const decl& tup)
                           { return tup.defined && str == tup.name; });
    if(it != structs.end())
        return &*it;
    }
    auto* p = (parent ? parent->find_struct_as_decl_helper(str) : nullptr);
    if(p)
        return p;
    //fall back to not yet defined ones (this is the case for forward declarations)
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&str](const decl& tup)
                           { return !tup.defined && str == tup.name; });
    if(it != structs.end())
        return &*it;
    }

    // uh oh
    return nullptr;
}

void scope::update_struct_references(const std::string& name, type::ptr* typ)
{
    auto set_it = data.find(std::type_index(typeid(struct_decl)));
    auto ptrsetit = data.find(std::type_index(typeid(types::ptr)));

    //nothing to update
    if(set_it == data.end() && ptrsetit == data.end())
        return;

    if(set_it != data.end())
    {
        // we are always a struct_decl here, thus the type is
        // assumed to be a mhc4::types::_struct
        for(auto& x : set_it->second)
        {
            assert(dynamic_cast<types::_struct*>(x.typ.get()));
            auto* strct = static_cast<types::_struct*>(x.typ.get());
            // we may assume to not be one of those anonymous structs
            assert(strct->has_name());

            if(!x.struct_definition && name == strct->name().userdata())
            {
                // we need updating
                x.struct_definition = typ;
            }
        }
    }
    if(ptrsetit != data.end())
    {
        // we are always a decl with a ptr here
        for(auto& x : ptrsetit->second)
        {
            assert(dynamic_cast<types::ptr*>(x.typ.get()));
            auto* pt = static_cast<types::ptr*>(x.typ.get());
            auto* base = pt->base_type();
            assert(base);
            auto* strct = dynamic_cast<types::_struct*>(base);
            assert(strct && strct->has_name());

            if(!x.struct_definition && name == strct->name().userdata())
            {
                // we need updating
                x.struct_definition = typ;
            }
        }
    }
}

scope scope::create_with_structs(scope& s, std::deque<scope>& toInherit)
{
    toInherit.emplace_back(s.parent
                           ?
                           scope::create_with_structs(*s.parent, toInherit)
                           :
                           scope::create());
    for(auto& x : s.structs)
    {
        toInherit.back().structs.emplace_back(decl{x.typ->copy(), x.name, x.defined, nullptr, ""});
        toInherit.back().structs.back().struct_definition = &toInherit.back().structs.back().typ;
    }

    return toInherit.back().inherit();
}

std::size_t scope::find_struct_height(const type* t)
{
    assert(t);
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&t](const decl& tup)
                           { return tup.defined && *t == *tup.typ; });
    if(it != structs.end())
        return 0;
    }
    std::size_t par = (parent ? parent->find_struct_height(t) : static_cast<std::size_t>(-1));
    if(par != static_cast<std::size_t>(-1))
        return par + 1;

    //fall back to not yet defined ones
    {
    auto it = std::find_if(structs.begin(), structs.end(),
                           [&t](const decl& tup)
                           { return !tup.defined && *t == *tup.typ; });
    if(it != structs.end())
        return 0;
    }

    // uh oh
    return -1;
}

}

}
