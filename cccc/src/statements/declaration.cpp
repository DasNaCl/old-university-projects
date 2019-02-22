#include <statements/declaration.hpp>

#include <types/function.hpp>
#include <types/integral.hpp>
#include <types/struct.hpp>
#include <types/thunk.hpp>
#include <types/ptr.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <llvm/IR/GlobalVariable.h>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace statements
{

declaration::declaration(const source_location& loc, type::ptr t)
    : statement_semicolon_printer(loc), t(std::move(t)), name(util::nullopt), expr(util::nullopt)
{  }

declaration::declaration(const source_location& loc,
                         type::ptr t, token name)
    : statement_semicolon_printer(loc), t(std::move(t)), name(name), expr(util::nullopt)
{  }

declaration::declaration(const source_location& loc,
                         type::ptr t, token name, expression::ptr expr)
    : statement_semicolon_printer(loc), t(std::move(t)), name(name), expr(std::move(expr))
{  }

void declaration::print(std::size_t depth) const
{
    if(name)
    {
        t->print(name.get().userdata(), depth);
        if(expr)
        {
            std::cout << " = ";
            expr.get()->print_expr();
        }
    }
    else
        t->print(depth);
}

void declaration::infer(semantics::scope& s) const
{
    try
    {
        t->infer(s);
    }
    catch(std::invalid_argument& arg)
    {
        std::string str = arg.what();
        auto it = str.find(": error: ");
        if(it == std::string::npos)
        {
            throw std::invalid_argument(loc.filepath() + ":" +
                                        std::to_string(loc.line()) + ":" +
                                        std::to_string(loc.column()) + ":" +
                                        " error: " + arg.what());
        }
        else
            throw std::invalid_argument(arg.what());
    }
    if(s.is_external() && expr.has())
    {
        semantics::throw_error(expr.get()->get_location(), "initialised global value");
    }
    if(name.has())
    {
        if(t->is_function())
            s.insert_function(name.get(), t, false);
        else
            s.insert_decl(t, name.get());
    }
    // infer expr if existent
    if(expr.has())
    {
        auto typ = expr.get()->infer(s);
        if(t->is_arithmetic() && typ->is_arithmetic())
        {
            // ok
        }
        else if(t->is_struct() && typ->is_struct() && *t == *typ)
        {
            // ok
        }
        else if(t->is_pointer() && typ->is_pointer()
                && (types::util::thunk_cast<const types::ptr*>(t.get())->dereference()->is_void()
                    || types::util::thunk_cast<const types::ptr*>(typ.get())->dereference()->is_void()))
        {
            // ok
        }
        else if(t->is_pointer() && typ->is_pointer() && (*t == *typ || types::util::thunk_cast<const types::ptr*>(typ.get())->dereference()->is_void()))
        {
            // ok
        }
        else if(t->is_pointer() && typ->is_function() && (*t == *typ))
        {
            // ok
        }
        else if(t->is_pointer() && expr.get()->is_null())
        {
            // ok
        }
        else
        {
            throw std::invalid_argument(loc.filepath() + ":" +
                                        std::to_string(loc.line()) + ":" +
                                        std::to_string(loc.column()) + ":" +
                                        " error: Type mismatch while assigning!");
        }
    }
}

void declaration::generate_ir(irconstructer& irc, IRScope& irscope)
{
    // no op
    if(!name.has() && !t->is_struct())
        return;
    if(!t->is_function())
        generate_ir_for_nonfunc_type(irc, irscope);
    else
        generate_ir_for_func_type(irc, irscope);
}

void declaration::generate_ir_for_func_type(irconstructer& irc, IRScope& irscope)
{
    {
    llvm::Function* val = irscope.find_function(name.get().userdata());
    // sema ensured that this declaration is valid
    if(val != nullptr)
        return;
    }
    auto& fun = *static_cast<types::function*>(t.get());
    std::vector<llvm::Type*> args;

    bool is_void = fun.parameter_count() == 1 && fun.parameter_type(0)->is_void();

    for(std::size_t i = 0U; !is_void && i < fun.parameter_count(); ++i)
        args.push_back(fun.parameter_type(i)->copy()->generate_ir(irc, irscope));

    auto* return_t = fun.get_return_type()->copy()->generate_ir(irc, irscope);

    // we are never vararg
    llvm::FunctionType* func_t =
        llvm::FunctionType::get(return_t, args, false);

    llvm::Function* func =
        llvm::Function::Create(func_t, llvm::GlobalValue::ExternalLinkage,
                               name.get().userdata(), &irc.module);
    irscope.insert_function(name.get().userdata(), func);
}


void declaration::generate_ir_for_nonfunc_type(irconstructer& irc, IRScope& irscope)
{
    auto* val_t = t->generate_ir(irc, irscope);
    // we need this, if we are a struct
    if(!name.has())
        return;
    llvm::Value* toInsert = nullptr;
    if(irc.current_fun)
    {
        irc.reset_alloca();
        toInsert = irc._alloca.CreateAlloca(val_t);
    }
    else
    {
        // external declaration
        auto* val =
            new llvm::GlobalVariable(irc.module,
                                     val_t,
                                     false,
                                     llvm::GlobalValue::CommonLinkage,
                                     llvm::Constant::getNullValue(val_t),
                                     name.get().userdata());
        toInsert = val;

        assert(!expr.has());
    }

    if(expr.has())
    {
        // simply assign the value of expr to it
        auto* expval = expr.get()->make_rvalue(irc, irscope);
        auto* var = toInsert;

        auto* vv = expval;
        if(val_t->isIntegerTy())
        {
            vv = irc._builder.CreateZExtOrTrunc(expval, val_t, "convert-exp-to-var-type-in-declaration-integer");
        }
        else if(val_t->isPointerTy())
        {
            if(expr.get()->is_null())
                vv = llvm::Constant::getNullValue(val_t);
            else
                vv = irc._builder.CreatePointerCast(expval, val_t, "convert-rhs-to-lhs-type-in-declaration-pointer");
        }

        irc._builder.CreateStore(vv, var);
    }
    // insert it here to the scope, because we want to be
    // able to do stuff like:
    // int main(int i) { { int i = i + 8; return i; } }
    irscope.insert(name.get().userdata(), toInsert);
    irscope.insert_type(name.get().userdata(), val_t);
}

bool declaration::has_name() const noexcept
{
    return name.has();
}

bool declaration::has_expr() const noexcept
{
    return expr.has();
}

type::ptr declaration::rm_type()
{
    return std::move(t);
}

const token& declaration::get_name() const noexcept
{
    return name.get();
}

}

}
