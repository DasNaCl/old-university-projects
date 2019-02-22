#include <optimize/SCCP.hpp>
#include <optimize/SCCPOptimizer.hpp>

#include <llvm/Transforms/Utils/Local.h>
#include <llvm/IR/InstrTypes.h>

#include <iostream>

namespace mhc4
{

namespace pass
{

namespace ___mhc4_optimize_detail
{
static llvm::RegisterPass<SCCP> X("mhc4sccp", "mhc4sccp-pass", false, false);
}

char SCCP::ID = 0;

SCCP::SCCP()
    : FunctionPass(ID)
{  }

bool SCCP::runOnFunction(llvm::Function& f)
{
    if(skipFunction(f))
        return false;
    SCCPOptimizer sccp(f.getParent()->getDataLayout());
    sccp.make_executable(&f.front());
    for(auto& a : f.args())
        sccp.overapproximate(&a);

    bool undefs = true;
    while(undefs)
    {
        sccp.run();
        undefs = sccp.has_undefs(&f);
    }

    bool change_occured = false;
    for(auto &bb : f)
    {
        if(!sccp.is_executable(&bb))
        {
            // remove dead code
            change_occured |= (llvm::removeAllNonTerminatorAndEHPadInstructions(&bb) > 0);
            continue;
        }

        for(auto bit = bb.begin(); bit != bb.end(); )
        {
            auto* ins = &*bit++;
            if(ins->getType()->isVoidTy() || llvm::isa<llvm::TerminatorInst>(ins))
                continue;

            bool erase = true;
            llvm::Constant* constant = nullptr;
            if(ins->getType()->isStructTy())
            {
                auto* strct = static_cast<llvm::StructType*>(ins->getType());
                bool constant_struct = true;
                std::vector<llvm::Constant*> constants;
                for(std::size_t i = 0; i < strct->getNumElements(); ++i)
                {
                    const lattice_elem& latel = sccp.struct_lat(ins, i);
                    if(latel.is_top())
                        constant_struct = false;
                    constants.push_back(latel.is_constant()
                                        ?
                                        latel.constant()
                                        :
                                        llvm::UndefValue::get(strct->getElementType(i)));
                }
                if(constant_struct)
                    constant = llvm::ConstantStruct::get(strct, constants);
            }
            else
            {
                const lattice_elem& latel = sccp.value_lat(ins);
                if(latel.is_top())
                    erase = false;
                else
                    constant = (latel.is_constant()
                                ?
                                latel.constant()
                                :
                                llvm::UndefValue::get(ins->getType()));
            }
            if(constant)
            {
                ins->replaceAllUsesWith(constant);
                change_occured = true;
            }
            if(erase && llvm::isInstructionTriviallyDead(ins))
            {
                ins->eraseFromParent();
                change_occured = true;
            }
        }
    }
    return change_occured;
}

}

}
