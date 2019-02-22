#include <expressions/assign.hpp>

#include <types/array.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <ir/irconstructer.hpp>

#include <sema/analyzer.hpp>

namespace mhc4
{

namespace expressions
{

assign::assign(token tok, expression::ptr left, expression::ptr right)
    : binary_printer(tok, std::move(left), std::move(right))
{  }

type::ptr assign::_infer(semantics::scope& s)
{
    auto ltyp = lhs->infer(s);
    auto rtyp = rhs->infer(s);

    if(!lhs->is_l_value())
    {
        semantics::throw_error(op, "Left operand of \"=\" is not an lvalue!");
    }

    const bool arithmetic = ltyp->is_arithmetic() && rtyp->is_arithmetic();
    const bool array = ltyp->is_array() && rtyp->is_array();
    const bool pointer = ltyp->is_pointer() && rtyp->is_pointer();
    const bool composite = ltyp->is_struct() && rtyp->is_struct();
    const bool funcassign = ltyp->is_pointer() && rtyp->is_function();

    if(arithmetic || (ltyp->is_pointer() && rhs->is_null())
    || ((array || pointer || composite || funcassign) && *ltyp == *rtyp))
    {
        // ok
    }
    else if(pointer && (ltyp->is_void_ptr() || rtyp->is_void_ptr()))
    {
        // ok
    }
    else if((ltyp->is_pointer() && rtyp->is_array())
         || (ltyp->is_array() && rtyp->is_pointer()))
    {
        auto arr = types::util::thunk_cast<const types::array*>(ltyp->is_array() ? ltyp.get() : rtyp.get());
        auto ptr = types::util::thunk_cast<const types::ptr*>(ltyp->is_pointer() ? ltyp.get() : rtyp.get());

        if(*arr->base().get() == *ptr->dereference().get())
        {
            // ok
            return util::make_unique<types::ptr>(arr->base());
        }
        else
        {
            semantics::throw_error(op, "Type mismatch for operator \"=\"");
        }
    }
    else
    {
        semantics::throw_error(op, "Type mismatch for operator \"=\"");
    }
    return ltyp;
}

llvm::Value* assign::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).first;
}

llvm::Value* assign::make_lvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).second;
}

std::pair<llvm::Value*, llvm::Value*> assign::make_value(irconstructer& irc, IRScope& irscope)
{
    auto* t = lhs->make_lvalue(irc, irscope);
    auto* v = rhs->make_rvalue(irc, irscope);

    auto* lltyp = lhs->make_rvalue(irc, irscope)->getType();
    auto* vv = v;
    if(lltyp->isIntegerTy())
    {
        vv = irc.integer_conversion(v, lltyp);
    }
    else if(lltyp->isPointerTy())
    {
        if(rhs->is_null())
            vv = llvm::Constant::getNullValue(lltyp);
        else
            vv = irc._builder.CreatePointerCast(v, lltyp, "convert-rhs-to-lhs-type-in-assign-pointer");
    }
    irc._builder.CreateStore(vv, t);

    return std::make_pair(v, t);
}

}

}
