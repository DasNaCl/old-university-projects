#include <expressions/negation.hpp>

#include <ir/irconstructer.hpp>

namespace mhc4
{

namespace expressions
{

negation::negation(token op, bool postfix, expression::ptr operand)
    : unary_printer(op, postfix, std::move(operand))
{  }

llvm::Value* negation::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    auto* v = operand->make_rvalue(irc, irscope);
    return irc._builder.CreateICmpEQ(v, llvm::Constant::getNullValue(v->getType()));
}

void negation::make_cf(irconstructer& irc, IRScope& irscope, llvm::BasicBlock* trueBB, llvm::BasicBlock* falseBB)
{
    operand->make_cf(irc, irscope, falseBB, trueBB);
}

}

}
