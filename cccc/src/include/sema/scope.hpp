#pragma once

#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include <lex/token.hpp>

#include <types/type.hpp>

#ifdef __GNUG__
//to demangle type names
#include <cxxabi.h>
#include <cassert>
#include <util>
///////////////////
#endif

namespace mhc4
{

namespace types
{
    class function;
}

namespace semantics
{

template<typename T>
class already_declared_exception : public std::invalid_argument
{
private:
    std::unique_ptr<char, decltype(std::free)*> custom_make_unique(char* ptr)
    {
        return std::unique_ptr<char, decltype(std::free)*>(ptr, std::free);
    }
public:
    already_declared_exception(const std::string& name)
        : invalid_argument(("\"" + name + "\" with type ").c_str()),
          to_ret()
    {
        int status = 0;
        std::unique_ptr<char, decltype(std::free)*> mytype =
#ifdef __GNUG__
        custom_make_unique(abi::__cxa_demangle(typeid(T).name(), 0, 0, &status));
        assert(status == 0);
#else
        custom_make_unique("unknown");
#endif
        to_ret = (std::string(invalid_argument::what()) + mytype.get() +
                  " has already been declared.").c_str();
    }

    const char* what() const noexcept override
    {
        return to_ret.c_str();
    }

private:
    std::string to_ret;
};

class scope
{
public:
    static std::map<std::string, std::size_t> struct_counter;

    struct decl
    {
        type::ptr typ;
        std::string name;

        //relevant for struct and function declarations/definitions
        bool defined;

        // this is always NULL if !type->is_struct()
        // quite ugly, but the "nicest" quick-and-dirty solution
        // (sorry, Dijkstra....)
        type::ptr* struct_definition;
        std::string irname;
    };
private:
    explicit scope(scope* parent);

public:
    scope inherit();
    static scope create();

    bool is_external() const noexcept;

    void set_in_loop();
    bool in_loop() const;

    void set_in_function(types::function& func);
    bool in_function() const;
    types::function* get_function() const;

    template<typename T>
    bool has(const std::string& name) const;
    bool has_any(const std::string& name) const;
    bool has_label(const std::string& name) const;

    template<typename T>
    void insert(const token& name, const type::ptr& typ);

    decl* find_decl_for_var(const token& tok);

    void insert_decl(const type::ptr& typ, const token& tok);
    void insert_struct(const token& tok, const type::ptr& typ, bool defined);
    void insert_function(const token& tok, const type::ptr& typ, bool defined);
    void insert_label(const token& tok);
    void insert_goto(const token& tok);

    void check_goto();

    type::ptr find_struct(const type* typ);
    decl* find_struct_as_decl(const type* typ);

    scope* get_parent()
    { return parent; }

    static scope create_with_structs(scope& s, std::deque<scope>& toInherit);

    std::size_t find_struct_height(const type* t);
    type::ptr var_struct_definition(const std::string& name) const;
private:
    decl* find_struct_as_decl_helper(const std::string& str);

    template<typename T>
    std::size_t height_of_var(const std::string& str) const;

    decl* get_closest_decl(const std::string& str);
    type::ptr* find_struct_ptr(const type::ptr& typ);
    type::ptr* get_closest_struct_ptr(const std::string& str);
    void update_struct_references(const std::string& name, type::ptr* typ);
private:
    scope* parent;

    std::map<std::type_index, std::vector<decl>> data;
    std::deque<decl> structs;
    std::deque<decl> functions;
    std::vector<std::string> labels;
    std::vector<token> gotos;

    bool hasloop;
    types::function *infunction;
};

}

}

namespace mhc4
{

namespace semantics
{

// need definitions outside

template<typename T>
bool scope::has(const std::string& name) const
{
    auto set_it = data.find(std::type_index(typeid(T)));
    if(set_it == data.end())
    {
        if(parent)
            return parent->has<T>(name);
        return false;
    }
    auto& set = set_it->second;
    auto it = std::find_if(set.begin(), set.end(),
                           [&name](const decl& p)
                           { return name == p.name; });
    if(it != set.end())
        return true;
    if(parent)
        return parent->has<T>(name);
    return false;
}

template<typename T>
void scope::insert(const token& name, const type::ptr& typ)
{
    // only check if type was already declared in current scope
    // anything else is shadowing!
    bool any = false;
    for(const auto& pair: data)
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
        already_declared_exception<T> ex(name.userdata());
        auto& loc = name.get_location();
        throw std::invalid_argument(loc.filepath() + ":"
                                    + std::to_string(loc.line()) + ":"
                                    + std::to_string(loc.column()) + ":"
                                    + " error: " + ex.what());
    }
    auto& set = data[std::type_index(typeid(T))];
    set.push_back(decl{typ->copy(), name.userdata(), false, nullptr, ""});
}

template<typename T>
std::size_t scope::height_of_var(const std::string& str) const
{
    const auto& vec_it = data.find(std::type_index(typeid(T)));
    if(vec_it == data.end())
    {
        if(parent)
        {
            const std::size_t res = parent->height_of_var<T>(str);
            return (res != static_cast<std::size_t>(-1) ? 1 + res : res);
        }
        // not found
        return -1;
    }
    auto& vec = vec_it->second;
    auto it = std::find_if(vec.begin(), vec.end(),
                           [&str](const decl& p)
                           { return str == p.name; });
    if(it != vec.end())
        return 0;
    else if(parent)
    {
        const std::size_t res = parent->height_of_var<T>(str);
        return (res != static_cast<std::size_t>(-1) ? 1 + res : res);
    }

    // not found
    return -1;
}

}

}
