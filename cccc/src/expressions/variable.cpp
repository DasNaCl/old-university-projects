#include <expressions/variable.hpp>

#include <ir/irconstructer.hpp>

#include <sema/scope.hpp>

#include <cassert>

namespace mhc4
{

namespace expressions
{

variable::variable(token tok)
    : primary_printer(tok)
{  }

type::ptr variable::_infer(semantics::scope& s)
{
    auto a = s.find_decl_for_var(tok);
    assert(a);
    if(a->typ->is_struct())
    {
        assert(a->struct_definition);
        return (*a->struct_definition)->copy();
    }
    return a->typ->copy();
}

type::ptr variable::struct_definition(semantics::scope& s)
{
    auto a = s.find_decl_for_var(tok);
    assert(a && a->struct_definition);
    return (*a->struct_definition)->copy();
}

token variable::name() const
{
    return tok;
}

llvm::Value* variable::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    auto* v = make_lvalue(irc, irscope);
    if(__expr_typ->is_function())
    {
        // functions are not loaded again!!
        return v;
    }
    return irc._builder.CreateLoad(v);
}

llvm::Value* variable::make_lvalue(irconstructer& irc, IRScope& irscope)
{
    if(irscope.has_val(tok.userdata()))
        return irscope.find(tok.userdata());
    return irscope.find_function(tok.userdata());
}

bool variable::is_primary() const noexcept
{
    return true;
}

bool variable::is_l_value() const noexcept
{
    return true;
}

}

}
