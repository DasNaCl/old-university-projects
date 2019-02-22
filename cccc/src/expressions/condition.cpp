#include <expressions/condition.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>

#include <types/integral.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <iostream>

namespace mhc4
{

namespace expressions
{

condition::condition(token tok, expression::ptr expr, expression::ptr consequence, expression::ptr alternative)
    : expression(tok.get_location()), tok(tok), expr(std::move(expr)),
      consequence(std::move(consequence)), alternative(std::move(alternative))
{  }

type::ptr condition::_infer(semantics::scope& s)
{
    auto first = expr->infer(s);

    // arrays can be converted to pointers implicitly
    if(!first->is_scalar() && !first->is_array())
    {
        semantics::throw_error(tok, "First operand is not a scalar type");
    }

    auto second = consequence->infer(s);
    auto third = alternative->infer(s);

    if(second->is_arithmetic() && third->is_arithmetic())
    {
        return util::make_unique<types::_integral>(tok::keyword_int);
    }
    if((second->is_void() && third->is_void())
    || (second->is_struct() && third->is_struct() && *second == *third))
    {
        // struct definitions should be same if struct == struct is ok
        return second;
    }
    else if((second->is_function() && third->is_pointer())
         || (second->is_pointer() && third->is_function()))
    {
        auto* ptr = types::util::thunk_cast<const types::ptr*>(third->is_pointer() ? third.get() : second.get());
        assert(ptr);

        auto* fun = (third->is_pointer() ? second.get() : third.get());

        auto t = ptr->dereference()->copy();
        if(*t == *fun)
        {
            return util::make_unique<types::ptr>(fun->copy());
        }
        else
        {
            semantics::throw_error(tok, "Operands don't have same function-type");
        }
    }
    else if(second->is_function() && third->is_function() && *second == *third)
    {
        return util::make_unique<types::ptr>(second->copy());
    }
    else if(second->is_pointer() && third->is_pointer())
    {
        auto* lptr = types::util::thunk_cast<const types::ptr*>(second.get());
        auto* rptr = types::util::thunk_cast<const types::ptr*>(third.get());
        assert(lptr && rptr);

        if(*second == *third)
        {
            return second;
        }
        else if(lptr->dereference()->is_void() || rptr->dereference()->is_void())
        {
            return (lptr->dereference()->is_void() ? rptr->copy() : lptr->copy());
        }
        else
        {
            semantics::throw_error(tok, "Operands don't have same struct or pointer type");
        }
    }
    else if(second->is_array() && third->is_array())
    {
        // this else if may as well be ignored.
        auto* larr = types::util::thunk_cast<const types::array*>(second.get());
        auto* rarr = types::util::thunk_cast<const types::array*>(third.get());
        if(larr->base()->is_arithmetic() && rarr->base()->is_arithmetic())
        {
            return util::make_unique<types::_integral>(tok::keyword_int);
        }
        else if(*larr->base() == *rarr->base())
        {
            return util::make_unique<types::ptr>(larr->base()->copy());;
        }
        else
        {
            semantics::throw_error(tok, "Operands don't have same array type");
        }
    }
    else if((second->is_array() && third->is_pointer())
         || (second->is_pointer() && third->is_array()))
    {
        // this else if block may as well be ignored
        auto* arr = types::util::thunk_cast<const types::array*>(second->is_array() ? second.get() : third.get());
        auto* ptr = types::util::thunk_cast<const types::ptr*>(second->is_pointer() ? second.get() : third.get());

        if(arr->base()->is_arithmetic() && ptr->dereference()->is_arithmetic())
        {
            return util::make_unique<types::_integral>(tok::keyword_int);
        }
        else if(*arr->base() == *ptr->dereference())
        {
            return util::make_unique<types::ptr>(arr->base()->copy());
        }
        else
        {
            semantics::throw_error(tok, "Operands don't agree on pointer type");
        }
    }
    else if(((second->is_pointer() || second->is_array()) && alternative->is_null())
          || (consequence->is_null() && (third->is_pointer() || third->is_array())))
    {
        return std::move((second->is_pointer() || second->is_array()) ? second : third);
    }
    else
    {
        semantics::throw_error(tok, "Invalid operands");
    }
    assert(false);
    return type::ptr(nullptr);
}

void condition::make_cf(irconstructer& irc, IRScope& irscope,
                        llvm::BasicBlock* trueBB,
                        llvm::BasicBlock* falseBB)
{
    llvm::BasicBlock* extratrueBB =
        llvm::BasicBlock::Create(irc.ctx, "extra-bb-logical-condition-true",
                                 irc.current_fun, nullptr);
    llvm::BasicBlock* extrafalseBB =
        llvm::BasicBlock::Create(irc.ctx, "extra-bb-logical-condition-false",
                                 irc.current_fun, nullptr);

    expr->make_cf(irc, irscope, extratrueBB, extrafalseBB);

    irc._builder.SetInsertPoint(extratrueBB);
    consequence->make_cf(irc, irscope, trueBB, falseBB);

    irc._builder.SetInsertPoint(extrafalseBB);
    alternative->make_cf(irc, irscope, trueBB, falseBB);
}

llvm::Value* condition::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    llvm::BasicBlock* aBB =
        llvm::BasicBlock::Create(irc.ctx, "a-bb-logical-condition-true",
                                 irc.current_fun, nullptr);
    llvm::BasicBlock* bBB =
        llvm::BasicBlock::Create(irc.ctx, "b-bb-logical-condition-false",
                                 irc.current_fun, nullptr);
    llvm::BasicBlock* mergeBB =
        llvm::BasicBlock::Create(irc.ctx, "merge-bb-logical-condition-false",
                                 irc.current_fun, nullptr);
    auto* myllvmtyp = get_llvm_type(irc, irscope);

    auto* v = expr->make_rvalue(irc, irscope);
    auto* c = irc._builder.CreateICmpNE(v, llvm::Constant::getNullValue(v->getType()));
    irc._builder.CreateCondBr(c, aBB, bBB);

    // emit consequence
    irc._builder.SetInsertPoint(aBB);
    auto* vc = consequence->make_rvalue(irc, irscope);

    if(myllvmtyp->isIntegerTy())
    {
        vc = irc.integer_conversion(vc, myllvmtyp);
    }
    else if(myllvmtyp->isPointerTy())
    {
        if(consequence->is_null())
            vc = llvm::Constant::getNullValue(myllvmtyp);
        else
            vc = irc._builder.CreatePointerCast(vc, myllvmtyp, "condition-ptr-cast-abb");
    }
    irc._builder.CreateBr(mergeBB);
    aBB = irc._builder.GetInsertBlock();

    // emit alternative
    irc._builder.SetInsertPoint(bBB);
    auto* va = alternative->make_rvalue(irc, irscope);

    if(myllvmtyp->isIntegerTy())
    {
        va = irc.integer_conversion(va, myllvmtyp);
    }
    else if(myllvmtyp->isPointerTy())
    {
        if(alternative->is_null())
            va = llvm::Constant::getNullValue(myllvmtyp);
        else
            va = irc._builder.CreatePointerCast(va, myllvmtyp, "condition-ptr-cast-abb");
    }

    irc._builder.CreateBr(mergeBB);
    bBB = irc._builder.GetInsertBlock();

    // choose value between consequence and alternative
    irc._builder.SetInsertPoint(mergeBB);
    llvm::PHINode* pn = irc._builder.CreatePHI(__expr_typ->generate_ir(irc, irscope), 2, "PHI-conditional");
    pn->addIncoming(vc, aBB);
    pn->addIncoming(va, bBB);

    return pn;
}

void condition::print() const
{
    expr->print_expr();
    std::cout << " " << tok.userdata() << " ";
    consequence->print_expr();
    std::cout << " : ";
    alternative->print_expr();
}

bool condition::is_l_value() const noexcept
{
    return false;
}

}

}
