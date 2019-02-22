#pragma once

#include <lex/source_location.hpp>
#include <lex/token_kind.hpp>

#include <ir/IRScope.hpp>

#include <llvm/IR/Type.h>

#include <util>

#include <memory>

namespace mhc4
{

class irconstructer;

namespace semantics
{
    class scope;
}

class type
{
public:
    using ptr = std::unique_ptr<type>;
protected:
    explicit type();
public:
    virtual ~type();

    virtual bool operator==(const type& typ) const = 0;

    void set_location(const source_location& loc);
    const source_location& get_location() const;

    virtual type* get_any_unprepared_thunk() noexcept = 0;

    virtual void print(std::size_t depth = 0) const = 0;
    virtual void print(std::string name, std::size_t depth = 0) const = 0;

    virtual void infer(semantics::scope& s) = 0;

    virtual llvm::Type* generate_ir(irconstructer& ir, IRScope& irscope) = 0;

    bool is_void_ptr() const noexcept;
    bool is_aggregate() const noexcept;
    bool is_arithmetic() const noexcept;
    bool is_scalar() const noexcept;

    virtual bool is_void() const noexcept = 0;
    virtual bool is_pointer() const noexcept = 0;

    virtual bool is_integral() const noexcept = 0;
    virtual bool is_function() const noexcept = 0;
    virtual bool is_array() const noexcept = 0;
    virtual bool is_struct() const noexcept = 0;
    virtual bool is_thunk() const noexcept = 0;

    virtual type::ptr copy() const = 0;
protected:
    source_location location;
};

namespace types
{

bool is_type(tok::token_kind kind) noexcept;

}

using type_opt = util::optional<type::ptr>;

}
