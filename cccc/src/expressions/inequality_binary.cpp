#include <expressions/inequality_binary.hpp>

#include <ir/irconstructer.hpp>

#include <sema/analyzer.hpp>

#include <types/integral.hpp>
#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <cassert>

namespace mhc4
{

namespace expressions
{

inequality::inequality(token tok, expression::ptr left, expression::ptr right)
    : binary_printer(tok, std::move(left), std::move(right))
{  }

type::ptr inequality::_infer(semantics::scope& s)
{
    auto ltyp = lhs->infer(s);
    auto rtyp = rhs->infer(s);

    const bool arithmetic = ltyp->is_arithmetic() && rtyp->is_arithmetic();
    const bool pointer = ltyp->is_pointer() && rtyp->is_pointer();
    const bool array = ltyp->is_array() && rtyp->is_array();

    if(arithmetic || ((array || pointer) && *ltyp == *rtyp))
    {
        // ok
    }
    else if((ltyp->is_array() && rtyp->is_pointer())
         || (ltyp->is_pointer() && rtyp->is_array()))
    {
        auto arr = types::util::thunk_cast<const types::array*>(ltyp->is_array() ? ltyp.get() : rtyp.get());
        auto ptr = types::util::thunk_cast<const types::ptr*>(ltyp->is_pointer() ? ltyp.get() : rtyp.get());

        if(*arr->base().get() == *ptr->dereference().get())
        {
            return util::make_unique<types::_integral>(tok::keyword_int);
        }
        else
        {
            semantics::throw_error(op, "Type mismatch for operator \"=\"");
        }
    }
    else
    {
        semantics::throw_error(op, "Operands of \"" + op.userdata() + "\" shall be either both real types or pointer types to compatible object types.");
    }
    return util::make_unique<types::_integral>(tok::keyword_int);
}

llvm::Value* inequality::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    const bool null_involved = (lhs->__get_expr_typ()->is_pointer() && rhs->is_null())
        || (lhs->is_null() && rhs->__get_expr_typ()->is_pointer());

    auto* l = lhs->make_rvalue(irc, irscope);
    auto* r = rhs->make_rvalue(irc, irscope);

    if(l->getType()->isIntegerTy() && r->getType()->isIntegerTy())
    {
        l = irc.integer_promotion(l);
        r = irc.integer_promotion(r);
    }
    else if(null_involved)
    {
        if(lhs->is_null())
            l = llvm::Constant::getNullValue(r->getType());
        else
            r = llvm::Constant::getNullValue(l->getType());
    }

    switch(op.get_kind())
    {
    case tok::lessequal:    return irc._builder.CreateICmpSLE(l, r);
    case tok::less:         return irc._builder.CreateICmpSLT(l, r);
    case tok::greaterequal: return irc._builder.CreateICmpSGE(l, r);
    case tok::greater:      return irc._builder.CreateICmpSGT(l, r);
    default: assert(false); return nullptr;
    }
}

}

}
