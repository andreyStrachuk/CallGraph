#include <llvm/Pass.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/AssemblyAnnotationWriter.h>
#include <vector>
using namespace llvm;

namespace {

  std::error_code EC;
  raw_fd_ostream *dumpFile;

  void openDumpSession () {
    dumpFile = new raw_fd_ostream("dump_dot.txt", EC);

    *dumpFile << "digraph D {\n";
  }

  void closeDumpSession () {
    *dumpFile << "}\n";

    dumpFile->close();
    delete dumpFile;
  }

  struct InstructionPass : public FunctionPass {
    static char ID;

    InstructionPass() : FunctionPass(ID) {
      openDumpSession();
    }

    ~InstructionPass () {
      closeDumpSession();
    }

    std::vector<StringRef> funcNames;

    virtual bool runOnFunction(Function &F) override {
      funcNames.push_back(F.getName());
      *dumpFile << (uint64_t)&F << " [fillcolor=cyan, style=\"filled\", label=\"" << F.getName() << "\"];\n";

      LLVMContext &ctx = F.getContext();
      IRBuilder<> builder(ctx);
      Type *retType = Type::getVoidTy(ctx);

      std::vector<Type *> callParams = {
        builder.getInt64Ty(),
        builder.getInt64Ty()
      };

      FunctionType *funcType = FunctionType::get(retType, callParams, false);
      FunctionCallee collectData = F.getParent()->getOrInsertFunction("collectData", funcType);

      for (auto &B : F) {
        for (auto &I : B) {

          if (auto *call = dyn_cast<CallInst>(&I)) {
            Function *calledFunc = call->getCalledFunction();
            StringRef calledFuncName = calledFunc->getName();

            if (std::find(funcNames.begin(), funcNames.end(), calledFuncName) == 
                funcNames.end()) {
              funcNames.push_back(calledFuncName);
              *dumpFile << (uint64_t)(calledFunc) << " [fillcolor=cyan, style=\"filled\", label=\"" << calledFuncName << "\"];\n";
            }
            
            builder.SetInsertPoint(call);

            Value *callerAddr = ConstantInt::get(builder.getInt64Ty(), (int64_t)&F);
            Value *calleeAddr = ConstantInt::get(builder.getInt64Ty(), (int64_t)calledFunc);

            Value *args[] = {callerAddr, calleeAddr};

            builder.CreateCall(collectData, args);
          }
          
        }
      }

      return true;
    }
  };
}

char InstructionPass::ID = 0;

static void registerInstrPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new InstructionPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerInstrPass);
