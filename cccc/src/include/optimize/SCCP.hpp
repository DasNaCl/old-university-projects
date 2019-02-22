#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Function.h>

#include <queue>

namespace mhc4
{

namespace pass
{

class SCCP : public llvm::FunctionPass
{
public:
    static char ID;
public:
    explicit SCCP();

    bool runOnFunction(llvm::Function& f) override;
};

inline llvm::Pass* createSparseConditionalConstantPropagationPass()
{
    return new SCCP();
}

}

}
