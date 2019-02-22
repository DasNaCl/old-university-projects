#include <expressions/function_call.hpp>

#include <sema/analyzer.hpp>

#include <types/function.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace expressions
{

function_call::function_call(token tok, expression::ptr callee, std::vector<expression::ptr> args)
    : expression(tok.get_location()), tok(tok), callee(std::move(callee)), args(std::move(args))
{  }

type::ptr function_call::_infer(semantics::scope& s)
{
    auto type = callee->infer(s);

    types::function* ftyp = nullptr;
    if(!type->is_function())
    {
        // it might be a ptr to a function....
        if(type->is_pointer())
        {
            auto* pt = types::util::thunk_cast<const types::ptr*>(type.get());
            assert(pt);
            auto* b = pt->dereference().get();
            assert(b);

            if(b->is_function())
            {
                ftyp = dynamic_cast<types::function*>(b);
                assert(ftyp);
            }
            else
                semantics::throw_error(tok, "Can not call on something which is not a function or function pointer");
        }
        else
            semantics::throw_error(tok, "Can not call on something which is not a function or function pointer");
    }
    ftyp = ftyp ? ftyp : dynamic_cast<types::function*>(type.get());
    assert(ftyp);

    auto parsize = ftyp->parameter_count();
    const bool is_actually_none = parsize == 1 && args.empty() && ftyp->parameter_type(0U)->is_void();
    if(parsize < args.size())
    {
        semantics::throw_error(tok, "Function demands less arguments!");
    }
    else if(parsize > args.size() && !(is_actually_none))
    {
        semantics::throw_error(tok, "Function demands more arguments!");
    }
    // check if arg types match
    if(!is_actually_none)
    for(std::size_t i = 0U; i < std::min(parsize, args.size()); ++i)
    {
        auto& to_match = ftyp->parameter_type(i);
        auto match_with = args[i]->infer(s)->copy();

        if((to_match->is_arithmetic() && match_with->is_arithmetic())
        || (to_match->is_struct() && match_with->is_struct() && *to_match == *match_with)
        || (to_match->is_pointer() && match_with->is_pointer() && *to_match == *match_with)
           || (to_match->is_pointer() && args[i]->is_null()))
        {
            // ok
        }
        else if(to_match->is_pointer() && match_with->is_pointer()
             && (types::util::thunk_cast<const types::ptr*>(to_match.get())->dereference()->is_void()
                 || types::util::thunk_cast<const types::ptr*>(match_with.get())->dereference()->is_void()))
        {
            // ok
        }
        else
        {
            semantics::throw_error(tok, "Function argument types mismatch!");
        }
    }
    return ftyp->get_return_type()->copy();
}

void function_call::print() const
{
    callee->print_expr();
    std::cout << "(";
    for(std::size_t i = 0; i < args.size(); ++i)
    {
        auto& arg = args[i];
        arg->print_expr();

        if(i + 1 < args.size())
            std::cout << ", ";
    }
    std::cout << ")";
}

llvm::Value* function_call::make_rvalue(irconstructer& irc, IRScope& irscope)
{
    auto* call_v = callee->make_rvalue(irc, irscope);
    std::vector<llvm::Value*> args_v;
    std::size_t i = 0;
    for(auto& x : args)
    {
        auto* t = x->make_rvalue(irc, irscope);
        auto& func = callee->__get_expr_typ();
        const types::function* f = nullptr;

        if(func->is_function())
            f = static_cast<const types::function*>(func.get());
        else
        {
            auto* ptr = static_cast<const types::ptr*>(func.get());
            assert(ptr->pointer_count() == 1);

            auto* x = ptr->base_type();
            assert(x->is_function());

            f = static_cast<const types::function*>(x);
        }
        if(i < f->parameter_count())
        {
            auto& typ = f->parameter_type(i);
            if(typ->is_arithmetic())
            {
                t = irc.integer_conversion(t, typ->generate_ir(irc, irscope));
            }
            else if(typ->is_pointer())
            {
                if(x->is_null())
                    t = llvm::Constant::getNullValue(typ->generate_ir(irc, irscope));
                else
                    t = irc._builder.CreatePointerCast(t, typ->generate_ir(irc, irscope), "pointercastforfunccall");
            }
        }
        args_v.push_back(t);
        ++i;
    }

    llvm::CallInst* v = irc._builder.CreateCall(call_v, args_v);

    {
        auto* ttyp = call_v->getType();
        auto* func = static_cast<llvm::FunctionType*>(ttyp);

        if(func->getReturnType()->isVoidTy())
            return nullptr;
    }
    return v;
}

bool function_call::is_l_value() const noexcept
{
    return false;
}

}

}
