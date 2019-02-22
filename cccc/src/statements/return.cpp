#include <statements/return.hpp>

#include <types/function.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <iostream>

namespace mhc4
{

namespace statements
{

_return::_return(const source_location& loc)
    : statement_semicolon_printer(loc), expr(util::nullopt)
{  }

_return::_return(const source_location& loc, expression::ptr expr)
    : statement_semicolon_printer(loc), expr(std::move(expr))
{  }

void _return::print(std::size_t) const
{
    std::cout << "return";

    if(expr)
    {
        std::cout << " ";
        expr.get()->print_expr();
    }
}

void _return::infer(semantics::scope& s) const
{
    if(!s.in_function())
    {
        throw semantics::broken_semantic_exception(loc, "return outside of a function!");
    }

    types::function& func = *s.get_function();
    if(func.returns_void() && expr.has())
    {
        throw semantics::broken_semantic_exception(loc, "return expression inside of void function");
    }
    else if(!func.returns_void() && !expr.has())
    {
        throw semantics::broken_semantic_exception(loc, "return must return something here");
    }

    if(expr.has())
    {
        expr.get()->infer(s);
    }
}

void _return::generate_ir(irconstructer& irc, IRScope& irscope)
{
    if(expr.has())
    {
        auto* v = expr.get()->make_rvalue(irc, irscope);

        auto* retty = irc._builder.getCurrentFunctionReturnType();
        if(retty->isIntegerTy())
        {
            v = irc.integer_conversion(v, irc._builder.getCurrentFunctionReturnType());
        }
        else if(retty->isPointerTy())
        {
            if(expr.get()->is_null())
                v = llvm::Constant::getNullValue(retty);
            else
                v = irc._builder.CreatePointerCast(v, retty, "convert-return-value-to-function-return-pointer");
        }

        irc._builder.CreateRet(v);
    }
    else
        irc._builder.CreateRetVoid();
    llvm::BasicBlock* dead_block =
        llvm::BasicBlock::Create(irc.ctx, "DEAD_BLOCK",
                                 irc.current_fun, nullptr);
    irc._builder.SetInsertPoint(dead_block);
}

}

}
