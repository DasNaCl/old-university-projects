#include <expressions/expression.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

expression::expression(const source_location& loc)
    : loc(loc)
{  }

type::ptr expression::infer(semantics::scope& s)
{
    auto t = _infer(s);
    __expr_typ = t->copy();
    return t->copy();
}

const source_location& expression::get_location() const
{
    return loc;
}

bool expression::is_primary() const noexcept
{
    return false;
}

bool expression::is_null() const noexcept
{
    return false;
}

void expression::print_expr() const
{
    std::cout << "(";
    print();
    std::cout << ")";
}

void expression::make_cf(irconstructer& irc, IRScope& irscope, llvm::BasicBlock* trueBB, llvm::BasicBlock* falseBB)
{
    auto* rv = this->make_rvalue(irc, irscope);
    auto* cond = irc._builder.CreateICmpNE(rv, llvm::Constant::getNullValue(rv->getType()));

    irc._builder.CreateCondBr(cond, trueBB, falseBB);
}

llvm::Type* expression::get_llvm_type(irconstructer& irc, IRScope& irscope)
{
    return __expr_typ->generate_ir(irc, irscope);
}

const type::ptr& expression::__get_expr_typ() const
{
    return __expr_typ;
}

}
