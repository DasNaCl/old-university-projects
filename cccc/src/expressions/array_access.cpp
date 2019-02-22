#include <expressions/array_access.hpp>

#include <expressions/arithmetic_binary.hpp>

#include <sema/analyzer.hpp>

#include <ir/irconstructer.hpp>

#include <types/integral.hpp>
#include <types/thunk.hpp>
#include <types/array.hpp>
#include <types/ptr.hpp>

#include <iostream>

namespace mhc4
{

namespace expressions
{

array_access::array_access(token tok, expression::ptr from, expression::ptr at)
    : expression(tok.get_location()), op(tok), from(std::move(from)), at(std::move(at))
{

}

type::ptr array_access::_infer(semantics::scope& s)
{
    auto ltyp = from->infer(s);
    auto rtyp = at->infer(s);

    type::ptr deref(nullptr);
    if(ltyp->is_arithmetic() && rtyp->is_arithmetic())
    {
        // ok
        deref = util::make_unique<types::_integral>(tok::keyword_int);
    }
    else if((ltyp->is_arithmetic() && rtyp->is_pointer())
         || (ltyp->is_pointer() && rtyp->is_arithmetic()))
    {
        // ok
        deref = types::util::thunk_cast<const types::ptr*>((ltyp->is_pointer() ? ltyp : rtyp).get())->dereference()->copy();
    }
    else if((ltyp->is_arithmetic() && rtyp->is_array())
         || (ltyp->is_array() && rtyp->is_arithmetic()))
    {
        // ok
        deref = types::util::thunk_cast<const types::array*>((ltyp->is_array() ? ltyp : rtyp).get())->copy();
    }
    else
    {
        semantics::throw_error(op, "Operands of \"+\" need to be arithmetic types or one may be a pointer type.");
    }

    // now it's just like dereferencing deref
    if((!ltyp->is_pointer() && !ltyp->is_array()) && (!rtyp->is_pointer() && !rtyp->is_array()))
    {
        semantics::throw_error(op, "Can not dereference a non pointer type.");
    }
    if(deref->is_void())
    {
        semantics::throw_error(op, "void value not ignored as it ought to be.");
    }

    return deref->copy();
}

llvm::Value* array_access::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).first;
}

llvm::Value* array_access::make_lvalue(irconstructer& irc, IRScope& irscope)
{
    return make_value(irc, irscope).second;
}

void array_access::print() const
{
    from->print_expr();
    std::cout << "[";
    at->print_expr();
    std::cout << "]";
}

bool array_access::is_l_value() const noexcept
{
    return true;
}

std::pair<llvm::Value*, llvm::Value*> array_access::make_value(irconstructer& irc, IRScope& irscope)
{
    auto* t = from->make_rvalue(irc, irscope);
    auto* v = at->make_rvalue(irc, irscope);

    auto* i64 = irc.integer_conversion(v, irc._builder.getInt64Ty());

    std::vector<llvm::Value*> args;
    args.push_back(i64);

    auto* ith_el_a = irc._builder.CreateGEP(t, args);
    auto* ith_el_v = irc._builder.CreateLoad(ith_el_a);

    return std::make_pair(ith_el_v, ith_el_a);
}

}

}
