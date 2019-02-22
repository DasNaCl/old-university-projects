#include <statements/function.hpp>
#include <statements/block.hpp>

#include <types/function.hpp>
#include <types/thunk.hpp>

#include <sema/analyzer.hpp>
#include <sema/scope.hpp>

#include <ir/irconstructer.hpp>

#include <llvm/IR/Verifier.h>

#include <iostream>
#include <cassert>

namespace mhc4
{

namespace statements
{

function::function(const source_location& loc, type::ptr functyp, token name,
                   const std::vector<std::pair<std::size_t, token>>& params,
                   statement::ptr body)
    : statement(loc), functyp(std::move(functyp)), name(name),
      parameter_names(params), body(std::move(body))
{  }

void function::print(std::size_t d) const
{
    const types::function* fn =
        (functyp->is_thunk()
         ?
         static_cast<const types::function*>(static_cast<const types::thunk*>(functyp.get())->get().get())
         :
         static_cast<const types::function*>(functyp.get()));
    std::vector<std::string> pars;
    for(const auto& p : parameter_names)
        pars.push_back(p.second.userdata());
    fn->print(name.userdata(), pars, d);
    std::cout << "\n";

    assert(body->is_block());
    dynamic_cast<block*>(body.get())->do_not_add_newline();
    body->print_statement(d);
}

void function::infer(semantics::scope& s) const
{
    try
    {
        functyp->infer(s);
    }
    catch(std::invalid_argument& arg)
    {

        throw std::invalid_argument(loc.filepath() + ":" +
                                    std::to_string(loc.line()) + ":" +
                                    std::to_string(loc.column()) + ":" +
                                    " error: " + arg.what());
    }
    auto& fun = *(static_cast<types::function*>(functyp.get()));

    // only add us if we weren't yet declared
    s.insert_function(name, functyp, true);

    auto z = s.inherit();
    for(const auto& pair : parameter_names)
    {
        z.insert_decl(fun.parameter_type(pair.first), pair.second);
    }
    z.set_in_function(fun);

    dynamic_cast<block*>(body.get())->infer_in_scope(z);
    z.check_goto();
}

void function::generate_ir(irconstructer& irc, IRScope& irscope)
{
    // we have no varargs, so third param is always false
    llvm::FunctionType* func_type = static_cast<llvm::FunctionType*>(functyp->generate_ir(irc, irscope));

    llvm::Function* func = nullptr;
    {
        llvm::Function* val = irscope.find_function(name.userdata());
    if(val == nullptr)
    {
        func = llvm::Function::Create(func_type,
                                      llvm::GlobalValue::ExternalLinkage,
                                      name.userdata(),
                                      &irc.module);
        irscope.insert_function(name.userdata(), func);
    }
    else
        func = val;
    }

    llvm::BasicBlock* func_entry =
        llvm::BasicBlock::Create(irc.ctx,
                                 "entry__" + name.userdata(),
                                 func,
                                 nullptr);
    // Fixing names of our arguments
    llvm::Function::arg_iterator func_arg_it = func->arg_begin();
    std::vector<std::pair<std::string, llvm::Argument*>> llvmargs;
    for(const auto& x : parameter_names)
    {
        func_arg_it->setName(x.second.userdata());
        llvmargs.push_back(std::make_pair(x.second.userdata(), func_arg_it));
        ++func_arg_it;
    }

    irc._builder.SetInsertPoint(func_entry);
    irc._alloca.SetInsertPoint(func_entry);

    auto z = irscope.inherit();
    for(const auto& x : llvmargs)
    {
        irc._alloca.SetInsertPoint(irc._alloca.GetInsertBlock(),
                                   irc._alloca.GetInsertBlock()->begin());
        llvm::Value* argv = irc._alloca.CreateAlloca(x.second->getType());
        irc._builder.CreateStore(x.second, argv);

        z.insert(x.first, argv);
        z.insert_type(x.first, x.second->getType());
    }

    irc.current_fun = func;

    // now just recursively generate the body
    dynamic_cast<block*>(body.get())->generate_ir_after_block(irc, z);

    irc.current_fun = nullptr;

    // emit a last return for the very last block
    // (which might not yet contain one, but should)
    if(!irc._builder.GetInsertBlock()->getTerminator())
    {
        llvm::Type *curfunret_t = irc._builder.getCurrentFunctionReturnType();
        if(curfunret_t->isVoidTy())
            irc._builder.CreateRetVoid();
        else
            irc._builder.CreateRet(llvm::Constant::getNullValue(curfunret_t));
    }


    llvm::verifyFunction(*func);
}

}

}
