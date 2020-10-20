#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include <string>
using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();
      // I'm just going to assume I can get printf



      Type *intType = Type::getInt32Ty(Ctx);

      // Declare C standard library printf
      std::vector<Type *> printfArgsTypes({Type::getInt8PtrTy(Ctx)});
      FunctionType *printfType = FunctionType::get(intType, printfArgsTypes, true);
      FunctionCallee printFunc = F.getParent()->getOrInsertFunction("printf", printfType);

      //FunctionCallee printFunc = F.getParent()->getFunction("printf");
      if (!printFunc) {
        errs() << "There is no printf, idk if you code will even compile" << "\n";
      }

      for (auto &B : F) {
        for (auto &I : B) {
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            // Insert at the point where the instruction `op` appears.
            IRBuilder<> builder(op);

            /*
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
            } */

            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

            std::string val ("At line ");
            std::string val_is (" we have a value of : %d\n");

            if (DILocation *Loc = op->getDebugLoc()) {
              errs() << "Line number: " << Loc->getLine() << "\n";
            } else {
              errs() << "failed to get location";
            }

            std::string msg = val + std::to_string(op->getDebugLoc()->getLine()) + val_is;

            //errs() << msg << "\n";

            Value *str = builder.CreateGlobalStringPtr(msg);

            //std::vector<Value *> argsV({str, op});
            Value* args[] = {str, op};
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
