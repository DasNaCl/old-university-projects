#include <optimize/SCCPOptimizer.hpp>

#include <llvm/Analysis/ConstantFolding.h>

namespace mhc4
{

namespace pass
{

SCCPOptimizer::SCCPOptimizer(const llvm::DataLayout& datalayout)
    : datalayout(datalayout)
{  }

void SCCPOptimizer::run()
{
    const auto visit_if_executable =
        [this](llvm::Value* val)
        {
            for(auto* u : val->users())
            {
                if(auto* i = llvm::dyn_cast<llvm::Instruction>(u))
                {
                    auto it = executable_blocks.find(i->getParent());
                    if(it != executable_blocks.end())
                        visit(*i);
                }
            }
        };

    while(!blocks.empty() || !instructions.empty() || !tops.empty())
    {
        while(!tops.empty())
        {
            llvm::Value* v = tops.front();
            tops.pop();
            visit_if_executable(v);
        }
        while(!instructions.empty())
        {
            llvm::Value* v = instructions.front();
            instructions.pop();

            if(v->getType()->isStructTy() || !value_lat(v).is_top())
            {
                visit_if_executable(v);
            }
        }
        while(!blocks.empty())
        {
            llvm::BasicBlock* bb = blocks.front();
            blocks.pop();

            visit(bb);
        }
    }
}

bool SCCPOptimizer::has_undefs(llvm::Function* f)
{
    assert(f);
    for(auto& bb : *f)
    {
        if(is_executable(&bb))
            continue;
        for(auto& ins : bb)
        {
            if(ins.getType()->isVoidTy())
                continue;
            else if(ins.getType()->isStructTy())
            {
                auto* strct = static_cast<llvm::StructType*>(ins.getType());
                for(std::size_t i = 0U; i < strct->getNumElements(); ++i)
                {
                    lattice_elem& late = struct_lat(&ins, i);
                    if(late.is_bot())
                        overapproximate(late, &ins);
                }
                continue;
            }
            lattice_elem& late = value_lat(&ins);
            if(!late.is_bot())
                continue;

            if(ins.getOperand(0)->getType()->isStructTy())
            {
                overapproximate(&ins);
                return true;
            }
            lattice_elem op0 = value_lat(ins.getOperand(0));
            lattice_elem op1;
            if(ins.getNumOperands() == 2)
            {
                if(ins.getOperand(1)->getType()->isStructTy())
                {
                    overapproximate(&ins);
                    return true;
                }
                op1 = value_lat(ins.getOperand(1));
            }

            using llvm::Instruction;
            switch(ins.getOpcode())
            {
            case Instruction::Add:
            case Instruction::Sub:
            case Instruction::Load:
            case Instruction::BitCast:
                break;

            case Instruction::ZExt:
            case Instruction::SExt:
            case Instruction::PtrToInt:
            case Instruction::IntToPtr:
            {
                set_as_prop_constant(&ins, llvm::Constant::getNullValue(ins.getType()));
                return true;
            }

            case Instruction::Mul:
            {
                if(op0.is_bot() && op1.is_bot())
                    break;
                set_as_prop_constant(&ins, llvm::Constant::getNullValue(ins.getType()));
                return true;
            }

            case Instruction::SDiv:
            {
                if(op1.is_bot())
                    break;
                if(op1.is_constant() && op1.constant()->isZeroValue())
                    break;

                set_as_prop_constant(&ins, llvm::Constant::getNullValue(ins.getType()));
                return true;
            }

            case Instruction::ICmp:
            {
                if(static_cast<llvm::ICmpInst*>(&ins)->isEquality())
                    break;
                overapproximate(&ins);
                return true;
            }

            default:
            {
                overapproximate(&ins);
                return true;
            }
            }

            auto* terminator = bb.getTerminator();
            if(auto* branch = llvm::dyn_cast<llvm::BranchInst>(terminator))
            {
                if(!branch->isConditional()
                || !value_lat(branch->getCondition()).is_bot())
                {
                    continue;
                }
                else if(llvm::isa<llvm::UndefValue>(branch->getCondition()))
                {
                    branch->setCondition(llvm::ConstantInt::getFalse(branch->getContext()));
                    make_executable(&bb, terminator->getSuccessor(1));
                    return true;
                }
                else
                {
                    set_as_prop_constant(branch->getCondition(),
                                       llvm::ConstantInt::getFalse(branch->getContext()));
                    return true;
                }
            }
        }
    }
    return false;
}

bool SCCPOptimizer::is_executable(llvm::BasicBlock* bb) const
{
    auto it = executable_blocks.find(bb);
    return it != executable_blocks.end();
}

bool SCCPOptimizer::make_executable(llvm::BasicBlock* bb)
{
    auto inserted_it = executable_blocks.insert(bb);
    if(!inserted_it.second)
        return false;

    blocks.push(bb);
    return true;
}

bool SCCPOptimizer::make_executable(llvm::BasicBlock* source, llvm::BasicBlock* dest)
{
    auto inserted_it = reachable_edges.insert({source, dest});
    if(!inserted_it.second)
        return false;
    if(!make_executable(dest))
    {
        for(auto& phi : dest->phis())
            visitPHINode(phi);
    }
    return true;
}

void SCCPOptimizer::overapproximate(llvm::Value* val)
{
    if(val->getType()->isStructTy())
    {
        auto* strct = static_cast<llvm::StructType*>(val->getType());
        for(std::size_t i = 0; i != strct->getNumElements(); ++i)
            overapproximate(struct_lat(val, i), val);
    }
    else
    {
        overapproximate(values[val], val);
    }
}

bool SCCPOptimizer::is_reachable(llvm::BasicBlock* source, llvm::BasicBlock* dest)
{
    assert(executable_blocks.find(dest) != executable_blocks.end());
    if(executable_blocks.find(source) == executable_blocks.end())
        return false;
    auto* terminator = source->getTerminator();

    auto* branch = llvm::dyn_cast<llvm::BranchInst>(terminator);

    // WE shouldn't have other terminators
    assert(branch);

    if(branch->isUnconditional())
        return true;
    lattice_elem latel = value_lat(branch->getCondition());

    auto* constant = (latel.is_constant()
                      ?
                      llvm::dyn_cast<llvm::ConstantInt>(latel.constant())
                      :
                      nullptr);
    if(!constant)
        return !latel.is_bot();
    return branch->getSuccessor(constant->isZero()) == dest;
}

void SCCPOptimizer::overapproximate(lattice_elem& el, llvm::Value* val)
{
    if(!el.set_as_top())
        return;

    if(el.is_top())
        tops.push(val);
    else
        instructions.push(val);
}

void SCCPOptimizer::set_as_constant(llvm::Value* val, llvm::Constant* constant)
{
    assert(!val->getType()->isStructTy());
    lattice_elem& latel = value_lat(val);
    if(!latel.set_as_constant(constant))
        return;
    if(latel.is_top())
        tops.push(val);
    else
        instructions.push(val);
}

void SCCPOptimizer::set_as_prop_constant(llvm::Value* val, llvm::Constant* constant)
{
    assert(!val->getType()->isStructTy());
    lattice_elem& latel = values[val];
    latel.set_as_prop_constant(constant);
    if(latel.is_top())
        tops.push(val);
    else
        instructions.push(val);
}

lattice_elem& SCCPOptimizer::struct_lat(llvm::Value* val, std::size_t pos)
{
    assert(val->getType()->isStructTy());
    assert(pos < static_cast<llvm::StructType*>(val->getType())->getNumElements());

    auto inserted_it = struct_values.insert({std::make_pair(val, pos), lattice_elem()});
    lattice_elem& latel = inserted_it.first->second;

    if(!inserted_it.second)
        return latel;

    // thanks: stackoverflow.com/questions/5315176
    if(auto* constant = llvm::dyn_cast<llvm::Constant>(val))
    {
        llvm::Constant* struct_element = constant->getAggregateElement(pos);
        if(struct_element)
            latel.set_as_constant(struct_element);
        else if(!llvm::isa<llvm::UndefValue>(struct_element))
            latel.set_as_top();
    }
    return latel;
}

lattice_elem& SCCPOptimizer::value_lat(llvm::Value* val)
{
    assert(!val->getType()->isStructTy());

    auto inserted_it = values.insert({val, lattice_elem()});
    lattice_elem& latel = inserted_it.first->second;

    if(!inserted_it.second)
        return latel;

    if(auto* constant = llvm::dyn_cast<llvm::Constant>(val))
    {
        if(!llvm::isa<llvm::UndefValue>(val))
            latel.set_as_constant(constant);
    }
    return latel;
}

std::vector<bool> SCCPOptimizer::successors(llvm::TerminatorInst& term)
{
    std::vector<bool> succ;
    succ.resize(term.getNumSuccessors());

    auto* branch = llvm::dyn_cast<llvm::BranchInst>(&term);
    // We do not have to deal with other types of branches
    assert(branch);

    if(branch->isUnconditional())
    {
        succ[0] = true;
        return succ;
    }

    lattice_elem latel = value_lat(branch->getCondition());
    auto* constant = (latel.is_constant()
                      ?
                      llvm::dyn_cast<llvm::ConstantInt>(latel.constant())
                      :
                      nullptr);
    if(!constant)
    {
        if(!latel.is_bot())
            succ[0] = succ[1] = true;
        return succ;
    }
    succ[constant->isZero()] = true;
    return succ;
}



void SCCPOptimizer::visitPHINode(llvm::PHINode& phi)
{
    if(phi.getType()->isStructTy())
    {
        overapproximate(&phi);
        return;
    }
    if(value_lat(&phi).is_top())
        return;
    llvm::Constant* constant = nullptr;
    for(std::size_t i = 0; i < phi.getNumIncomingValues(); ++i)
    {
        lattice_elem latel = value_lat(phi.getIncomingValue(i));

        const bool undef = latel.is_bot();
        const bool unreachable = !is_reachable(phi.getIncomingBlock(i), phi.getParent());
        if(undef || unreachable)
            continue;
        if(latel.is_top())
        {
            overapproximate(&phi);
            return;
        }
        if(!constant)
        {
            constant = latel.constant();
            continue;
        }

        if(latel.constant() != constant)
        {
            overapproximate(&phi);
            return;
        }
    }
    if(constant)
        set_as_constant(&phi, constant);
}

void SCCPOptimizer::visitReturnInst(llvm::ReturnInst& ret)
{
}

void SCCPOptimizer::visitTerminatorInst(llvm::TerminatorInst& term)
{
    auto succ = successors(term);
    auto* bb = term.getParent();

    for(std::size_t i = 0U; i < succ.size(); ++i)
        if(succ[i])
            make_executable(bb, term.getSuccessor(i));
}

void SCCPOptimizer::visitCastInst(llvm::CastInst& cast)
{
    lattice_elem latel = value_lat(cast.getOperand(0));
    if(latel.is_top())
        overapproximate(&cast);
    else if(latel.is_constant())
    {
        auto* constant = llvm::ConstantFoldCastOperand(cast.getOpcode(), latel.constant(), cast.getType(), datalayout);

        if(llvm::isa<llvm::UndefValue>(constant))
            return;
        set_as_constant(&cast, latel.constant());
    }
}

void SCCPOptimizer::visitBinaryOperator(llvm::Instruction& inst)
{
    auto p = std::make_pair(value_lat(inst.getOperand(0)), value_lat(inst.getOperand(1)));

    lattice_elem& latel = values[&inst];
    if(latel.is_top())
        return;
    if(p.first.is_constant() && p.second.is_constant())
    {
        auto* constant = llvm::ConstantExpr::get(inst.getOpcode(), p.first.constant(), p.second.constant());
        if(llvm::isa<llvm::UndefValue>(constant))
            return;
        if(latel.set_as_constant(constant))
        {
            if(latel.is_top())
                tops.push(&inst);
            else
                instructions.push(&inst);
        }
    }
    else
    {
        if(!p.first.is_top() && !p.second.is_top())
            return;
        if(inst.getOpcode() == llvm::Instruction::SDiv)
        {
            if(p.first.is_constant() && p.first.constant()->isNullValue())
            {
                if(latel.set_as_constant(p.first.constant()))
                {
                    if(latel.is_top())
                        tops.push(&inst);
                    else
                        instructions.push(&inst);
                }
            }
        }
        else if(inst.getOpcode() == llvm::Instruction::Mul)
        {
            lattice_elem* platel = (!p.first.is_top()
                                   ?
                                   &p.first
                                   :
                                   (!p.second.is_top()
                                    ?
                                    &p.second
                                    :
                                    nullptr));
            if(platel)
            {
                if(platel->is_bot())
                    return;
                if(platel->constant()->isNullValue())
                {
                    if(latel.set_as_constant(platel->constant()))
                    {
                        if(latel.is_top())
                            tops.push(&inst);
                        else
                            instructions.push(&inst);
                    }
                }
                else
                    overapproximate(&inst);
            }
            else
                overapproximate(&inst);
        }
        else
            overapproximate(&inst);
    }
}

void SCCPOptimizer::visitCmpInst(llvm::CmpInst& cmp)
{
    auto p = std::make_pair(value_lat(cmp.getOperand(0)), value_lat(cmp.getOperand(1)));

    lattice_elem& latel = values[&cmp];
    if(latel.is_top())
        return;

    if(p.first.is_constant() && p.second.is_constant())
    {
        auto* constant = llvm::ConstantExpr::getCompare(cmp.getPredicate(), p.first.constant(), p.second.constant());
        if(llvm::isa<llvm::UndefValue>(constant))
            return;
        if(latel.set_as_constant(constant))
        {
            if(latel.is_top())
                tops.push(&cmp);
            else
                instructions.push(&cmp);
        }
    }
    else
    {
        if(!p.first.is_top() && !p.second.is_top())
            return;
        overapproximate(&cmp);
    }
}

void SCCPOptimizer::visitGetElementPtrInst(llvm::GetElementPtrInst& gep)
{
    if(values[&gep].is_top())
        return;
    std::vector<llvm::Constant*> operands;
    for(std::size_t i = 0U; i < gep.getNumOperands(); ++i)
    {
        lattice_elem latel = value_lat(gep.getOperand(i));
        if(latel.is_bot())
            return;
        else if(latel.is_top())
        {
            overapproximate(&gep);
            return;
        }
        else
            operands.push_back(latel.constant());
    }
    auto* beg = operands[0];
    auto* constant = llvm::ConstantExpr::getGetElementPtr(gep.getSourceElementType(), beg, std::vector<llvm::Constant*>{operands.begin() + 1, operands.end()});
    if(llvm::isa<llvm::UndefValue>(constant))
        return;
    set_as_constant(&gep, constant);
}

void SCCPOptimizer::visitStoreInst(llvm::StoreInst& store)
{
    // TODO ?
}

void SCCPOptimizer::visitLoadInst(llvm::LoadInst& load)
{
    if(load.getType()->isStructTy())
    {
        overapproximate(&load);
        return;
    }

    lattice_elem op_latel = value_lat(load.getOperand(0));
    if(op_latel.is_bot())
        return;
    lattice_elem& latel = values[&load];
    if(latel.is_top())
        return;

    if(!op_latel.is_constant() || load.isVolatile())
    {
        overapproximate(latel, &load);
        return;
    }
    auto* val = op_latel.constant();
    if(llvm::isa<llvm::ConstantPointerNull>(val) && load.getPointerAddressSpace() == 0)
        return;

    auto* constant = llvm::ConstantFoldLoadFromConstPtr(val, load.getType(), datalayout);
    if(constant)
    {
        if(llvm::isa<llvm::UndefValue>(constant))
            return;
        if(latel.set_as_constant(constant))
        {
            if(latel.is_top())
                tops.push(&load);
            else
                instructions.push(&load);
        }
    }
    else
        overapproximate(latel, &load);
}

void SCCPOptimizer::visitCallSite(llvm::CallSite call)
{
    auto* f = call.getCalledFunction();
    auto* inst = call.getInstruction();

    if(inst->getType()->isVoidTy())
        return;

    const bool constant_foldable = llvm::canConstantFoldCallTo(call, f);
    if(f && f->isDeclaration() && !inst->getType()->isStructTy() && constant_foldable)
    {
        std::vector<llvm::Constant*> parameters;
        for(auto it = call.arg_begin(); it != call.arg_end(); ++it)
        {
            lattice_elem latel = value_lat(*it);

            if(latel.is_top())
            {
                overapproximate(inst);
                return;
            }
            else if(latel.is_constant())
                parameters.push_back(latel.constant());
            else
                return;
        }
        if(value_lat(inst).is_top())
            return;

        auto* constant = llvm::ConstantFoldCall(call, f, parameters);
        if(constant)
        {
            if(llvm::isa<llvm::UndefValue>(constant))
                return;
            set_as_constant(inst, constant);
            return;
        }
    }
    overapproximate(inst);
}

void SCCPOptimizer::visitCallInst(llvm::CallInst& inst)
{
    visitCallSite(&inst);
}

void SCCPOptimizer::visitInvokeInst(llvm::InvokeInst& inst)
{
    visitCallSite(&inst);
    visitTerminatorInst(inst);
}

void SCCPOptimizer::visitInstruction(llvm::Instruction& inst)
{
    overapproximate(&inst);
}

}

}
