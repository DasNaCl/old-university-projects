#pragma once

#include <statements/statement.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <vector>
#include <string>

namespace mhc4
{

class irconstructer
{
private:
    irconstructer(const std::string& name);
public:
    static void construct(const std::string& name, const std::vector<statement::ptr>& ast, bool optimize);

    llvm::Value* integer_promotion(llvm::Value* val);
    llvm::Value* integer_conversion(llvm::Value* val, llvm::Type* to);

    void reset_alloca();

    llvm::LLVMContext ctx;
    llvm::Module module;
    llvm::IRBuilder<> _builder;
    llvm::IRBuilder<> _alloca;

    llvm::Function* current_fun;
};

}
