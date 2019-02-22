#include <ir/irconstructer.hpp>
#include <ir/IRScope.hpp>

#include <optimize/SCCP.hpp>

// llvm
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>

namespace mhc4
{

irconstructer::irconstructer(const std::string& name)
    : ctx(), module(name, ctx), _builder(ctx), _alloca(ctx)
{  }

void irconstructer::construct(const std::string& name, const std::vector<statement::ptr>& ast, bool optimize)
{
    IRScope irscope = IRScope::create();
    irconstructer irc(name);
    for(const auto& node : ast)
    {
        irc.current_fun = nullptr;
        node->generate_ir(irc, irscope); // gen ir code for ast
    }
    for(auto& p : irscope.marked_gotos)
        assert(p.second.empty());

    llvm::verifyModule(irc.module);

    if(optimize)
    {
        auto fpm = util::make_unique<llvm::legacy::FunctionPassManager>(&irc.module);

        //add mem2reg pass
        fpm->add(llvm::createPromoteMemoryToRegisterPass());

        //add our own pass
        fpm->add(pass::createSparseConditionalConstantPropagationPass());
        //fpm->add(llvm::createSCCPPass());

        fpm->doInitialization();
        for(auto it = irc.module.begin(); it != irc.module.end(); ++it)
        {
            if(!it->isDeclaration())
                fpm->run(*it);
        }
        fpm->doFinalization();
    }

    const std::string base_filename = name.substr(name.find_last_of("/\\") + 1);
    const std::size_t len(base_filename.find_last_of('.'));
    const std::string iroutfile = base_filename.substr(0, len) + ".ll";

    std::error_code errc;
    llvm::raw_fd_ostream os(iroutfile, errc, llvm::sys::fs::OpenFlags::F_Text);
    irc.module.print(os, nullptr);
}

llvm::Value* irconstructer::integer_promotion(llvm::Value* val)
{
    return integer_conversion(val, _builder.getInt32Ty());
}

llvm::Value* irconstructer::integer_conversion(llvm::Value* val, llvm::Type* to)
{
    if(val->getType()->isIntegerTy(1))
        return _builder.CreateZExtOrTrunc(val, to, "integer-conversion");
    else
        return _builder.CreateSExtOrTrunc(val, to, "integer-conversion");
}

void irconstructer::reset_alloca()
{
    _alloca.SetInsertPoint(_alloca.GetInsertBlock(),
                           _alloca.GetInsertBlock()->begin());
}

}
