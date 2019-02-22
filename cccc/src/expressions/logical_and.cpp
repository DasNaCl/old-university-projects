#include <expressions/logical_and.hpp>

#include <ir/irconstructer.hpp>

#include <types/integral.hpp>

#include <sema/analyzer.hpp>

namespace mhc4
{

namespace expressions
{

logical_and::logical_and(token tok, expression::ptr left, expression::ptr right)
    : binary_printer(tok, std::move(left), std::move(right))
{  }

type::ptr logical_and::_infer(semantics::scope& s)
{
    auto ltyp = lhs->infer(s);
    auto rtyp = rhs->infer(s);

    if(!((ltyp->is_scalar() || ltyp->is_array())
      && (ltyp->is_array() || rtyp->is_scalar())))
    {
        semantics::throw_error(op, "Operands of \"||\" must be of scalar type");
    }
    return util::make_unique<types::_integral>(tok::keyword_int);
}

void logical_and::make_cf(irconstructer& irc, IRScope& irscope,
                          llvm::BasicBlock* trueBB,
                          llvm::BasicBlock* falseBB)
{
    llvm::BasicBlock* extraBB =
        llvm::BasicBlock::Create(irc.ctx, "extra-bb-logical-and",
                                 irc.current_fun, nullptr);
    lhs->make_cf(irc, irscope, extraBB, falseBB);
    irc._builder.SetInsertPoint(extraBB);
    rhs->make_cf(irc, irscope, trueBB, falseBB);
}

llvm::Value* logical_and::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    llvm::BasicBlock* trueBB =
        llvm::BasicBlock::Create(irc.ctx, "true-bb-logical-end",
                                 irc.current_fun, nullptr);
    llvm::BasicBlock* falseBB =
        llvm::BasicBlock::Create(irc.ctx, "false-bb-logical-end",
                                 irc.current_fun, nullptr);
    irc.reset_alloca();
    auto* endV = irc._alloca.CreateAlloca(irc._builder.getInt1Ty());
    irc._builder.CreateStore(irc._builder.getInt1(0), endV);
    auto* v0 = irc._builder.CreateLoad(endV);
    lhs->make_cf(irc, irscope, trueBB, falseBB);
    auto* oldBB = irc._builder.GetInsertBlock();

    irc._builder.SetInsertPoint(trueBB);
    auto* rhsv = rhs->make_rvalue(irc, irscope);
    auto* cond = irc._builder.CreateICmpNE(rhsv, llvm::Constant::getNullValue(rhsv->getType()));
    irc._builder.CreateStore(cond, endV);
    auto* v1 = irc._builder.CreateLoad(endV);
    irc._builder.CreateBr(falseBB);
    trueBB = irc._builder.GetInsertBlock();

    irc._builder.SetInsertPoint(falseBB);
    llvm::PHINode* phi = irc._builder.CreatePHI(irc._builder.getInt1Ty(), 2, "PHI-logical-and");
    phi->addIncoming(v0, oldBB);
    phi->addIncoming(v1, trueBB);

    return irc.integer_promotion(phi);
}

}

}
