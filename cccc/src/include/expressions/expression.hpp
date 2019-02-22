#pragma once

#include <lex/source_location.hpp>

#include <ir/IRScope.hpp>

#include <types/type.hpp>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <util>

namespace mhc4
{

class irconstructer;

namespace semantics
{
    class scope;
}

class expression
{
public:
    using ptr = std::unique_ptr<expression>;
protected:
    explicit expression(const source_location& loc);
public:
    virtual ~expression() noexcept
        {  }

    type::ptr infer(semantics::scope& s);
protected:
    virtual type::ptr _infer(semantics::scope& s) = 0;
public:
    const source_location& get_location() const;

    virtual bool is_primary() const noexcept;
    virtual bool is_null() const noexcept;

    virtual bool is_l_value() const noexcept = 0;

    virtual llvm::Value* make_rvalue(irconstructer& irc, IRScope& irscope) //= 0;
    { return nullptr; }
    virtual llvm::Value* make_lvalue(irconstructer& irc, IRScope& irscope) //= 0;
    { return nullptr; }
    virtual void make_cf(irconstructer& irc, IRScope& irscope,
                         llvm::BasicBlock* trueBB,
                         llvm::BasicBlock* falseBB);
    llvm::Type* get_llvm_type(irconstructer& irc, IRScope& irscope);

    virtual void print_expr() const;

    const type::ptr& __get_expr_typ() const;
protected:
    virtual void print() const = 0;
protected:
    source_location loc;

    type::ptr __expr_typ;
};

using expression_opt = util::optional<expression::ptr>;

}
