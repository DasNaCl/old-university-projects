#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

#include <string>
#include <map>

namespace mhc4
{

class IRScope
{
private:
    IRScope(IRScope* parent);
public:
    static IRScope create();
    IRScope inherit();

    bool has_val(const std::string& name);

    void insert(const std::string& name, llvm::Value* val);
    llvm::Value* find(const std::string& name);

    void insert_type(const std::string& name, llvm::Type* typ);
    llvm::Type* find_type(const std::string& name);

    void insert_struct(const std::string& name, llvm::StructType* struct_t);
    llvm::StructType* find_struct(const std::string& name);

    void insert_function(const std::string& name, llvm::Function* func);
    llvm::Function* find_function(const std::string& name);

    void insert_label(const std::string& name, llvm::BasicBlock* labelBB);
    llvm::BasicBlock* find_label(const std::string& name);

    void set_while_headBB(llvm::BasicBlock* head);
    void set_while_endBB(llvm::BasicBlock* end);
    llvm::BasicBlock* find_while_headBB();
    llvm::BasicBlock* find_while_endBB();

    IRScope& highest();
    std::map<std::string, std::vector<llvm::BasicBlock*>> marked_gotos;
private:
    std::map<std::string, llvm::Value*> data;
    std::map<std::string, llvm::Type*> type_data;
    std::map<std::string, llvm::StructType*> struct_data;
    std::map<std::string, llvm::Function*> function_data;
    std::map<std::string, llvm::BasicBlock*> label_data;

    llvm::BasicBlock* while_headBB;
    llvm::BasicBlock* while_endBB;
    IRScope* parent;
};

}
