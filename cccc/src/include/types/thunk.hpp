#pragma once

#include <lex/token_kind.hpp>

#include <types/type.hpp>

#include <cassert>

namespace mhc4
{

namespace types
{

// This is used for parsing only
class thunk : public type
{
public:
    using ptr = std::unique_ptr<thunk>;
public:
    explicit thunk();
    virtual ~thunk();

    bool is_prepared() const noexcept;
    void prepare(type::ptr d);
    const type::ptr& get() const noexcept;

    bool operator==(const type& typ) const override;

    type* get_any_unprepared_thunk() noexcept override;

    void print(std::size_t) const override;
    void print(std::string name, std::size_t) const override;

    void infer(semantics::scope& s) override;

    llvm::Type* generate_ir(irconstructer& irc, IRScope& irscope) override;

    bool is_void() const noexcept override;
    bool is_pointer() const noexcept override;
    bool is_integral() const noexcept override;
    bool is_function() const noexcept override;
    bool is_array() const noexcept override;
    bool is_struct() const noexcept override;
    bool is_thunk() const noexcept override;

    type::ptr copy() const override;
private:
    void test() const;
private:
    type::ptr t;
};

namespace util
{

template<typename T>
T thunk_cast(const type* t)
{
    assert(t);

    return (t->is_thunk()
            ?
            static_cast<T>(static_cast<const thunk*>(t)->get().get())
            :
            static_cast<T>(t));
}

template<typename T>
T non_const_thunk_cast(type* t)
{
    assert(t);

    return (t->is_thunk()
            ?
            static_cast<T>(static_cast<const thunk*>(t)->get().get())
            :
            static_cast<T>(t));
}
    
}
    
}

}
