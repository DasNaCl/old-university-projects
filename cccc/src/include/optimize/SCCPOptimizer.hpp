#pragma once

#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

#include <optimize/lattice_elem.hpp>

#include <queue>
#include <map>
#include <set>

namespace mhc4
{

namespace pass
{

class SCCPOptimizer : public llvm::InstVisitor<SCCPOptimizer>
{
public:
    template<typename T>
    using worklist_t = std::queue<T>;
public:
    explicit SCCPOptimizer(const llvm::DataLayout& datalayout);

    void run();

    bool has_undefs(llvm::Function* f);
    bool is_executable(llvm::BasicBlock* bb) const;

    bool make_executable(llvm::BasicBlock* bb);
    bool make_executable(llvm::BasicBlock* source, llvm::BasicBlock* dest);
    void overapproximate(llvm::Value* val);

    lattice_elem& struct_lat(llvm::Value* val, std::size_t pos);
    lattice_elem& value_lat(llvm::Value* val);
private:
    bool is_reachable(llvm::BasicBlock* source, llvm::BasicBlock* dest);

    void overapproximate(lattice_elem& el, llvm::Value* val);
    void set_as_constant(llvm::Value* val, llvm::Constant* constant);
    void set_as_prop_constant(llvm::Value* val, llvm::Constant* constant);

    std::vector<bool> successors(llvm::TerminatorInst& term);
public:
    void visitPHINode(llvm::PHINode& phi);
    void visitReturnInst(llvm::ReturnInst& ret);
    void visitTerminatorInst(llvm::TerminatorInst& term);
    void visitCastInst(llvm::CastInst& cast);
    void visitBinaryOperator(llvm::Instruction& inst);
    void visitCmpInst(llvm::CmpInst& cmp);
    void visitGetElementPtrInst(llvm::GetElementPtrInst& gep);
    void visitStoreInst(llvm::StoreInst& store);
    void visitLoadInst(llvm::LoadInst& load);
    void visitCallSite(llvm::CallSite call);

    void visitCallInst(llvm::CallInst& inst);
    void visitInvokeInst(llvm::InvokeInst& inst);

    void visitInstruction(llvm::Instruction& inst);

private:
    const llvm::DataLayout& datalayout;

    std::map<llvm::Value*, lattice_elem> values;
    std::map<std::pair<llvm::Value*, std::size_t>, lattice_elem> struct_values;

    std::set<llvm::BasicBlock*> executable_blocks;
    std::set<std::pair<llvm::BasicBlock*, llvm::BasicBlock*>> reachable_edges;

    worklist_t<llvm::Value*> tops;
    worklist_t<llvm::Value*> instructions;
    worklist_t<llvm::BasicBlock*> blocks;
};

}

}
