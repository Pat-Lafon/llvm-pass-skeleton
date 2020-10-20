#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();
      // I'm just going to assume I can get printf
      FunctionCallee printFunc = F.getParent()->getFunction("printf");
      if (!printFunc) {
        errs() << "There is no printf, idk if you code will even compile" << "\n";
      }

      for (auto &B : F) {
        for (auto &I : B) {
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            // Insert at the point where the instruction `op` appears.
            IRBuilder<> builder(op);

            // Make a multiply with the same operands as `op`.
            Value *lhs = op->getOperand(0);
            Value *rhs = op->getOperand(1);
            Value *mul = builder.CreateMul(lhs, rhs);

            op->replaceAllUsesWith(mul);
            if (op->use_empty()) {
              errs() << "is gone" << "\n";
              op-> eraseFromParent();
            } else {
              errs() << "whoops, still here" << "\n";
            }

            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

            Value *str = builder.CreateGlobalStringPtr("test\n", "str");
            // Insert a call to our function.
            Value* args[] = {str};
            builder.CreateCall(printFunc, args);

            // We modified the code.
            return true;
          }
        }
      }

      return false;
    }
  };
}


char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
