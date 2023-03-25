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
    dumpFile = new raw_fd_ostream("inter.txt", EC, sys::fs::OF_Append);
  }

  void closeDumpSession () {
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

    std::vector<u_int64_t> funcHashes;

    virtual bool runOnFunction(Function &F) override {
      uint64_t callerHash = std::hash<std::string>()(F.getName());
      StringRef callerName = F.getName();

      if (callerName == "main") {
        funcHashes.push_back(callerHash);
      }

      *dumpFile << callerHash << " [fillcolor=cyan, style=\"filled\", label=\"" << F.getName() << "\"];\n";

      LLVMContext &ctx = F.getContext();
      IRBuilder<> builder(ctx);
      Type *retType = Type::getVoidTy(ctx);

      std::vector<Type *> callParams = {
        builder.getInt64Ty(),
        builder.getInt64Ty()
      };

      FunctionType *funcType = FunctionType::get(retType, callParams, false);
      FunctionCallee collectData = F.getParent()->getOrInsertFunction("collectData", funcType);

      std::vector<Type *> retParams = {
        builder.getVoidTy() 
      };

      FunctionType *retFuncType = FunctionType::get(retType, retParams, false);
      FunctionCallee writeToFile = F.getParent()->getOrInsertFunction("writeToFile", retFuncType);

      std::vector<Type *> openParams = {
        builder.getVoidTy() 
      };

      FunctionType *openFuncType = FunctionType::get(retType, openParams, false);
      FunctionCallee openFile = F.getParent()->getOrInsertFunction("openFile", openFuncType);

      for (auto &B : F) {
        for (auto &I : B) {

          if (auto *call = dyn_cast<CallInst>(&I)) {
            Function *calledFunc = call->getCalledFunction();
            StringRef calledFuncName = calledFunc->getName();

            uint64_t calledHash = std::hash<std::string>()(calledFuncName);

            if (std::find(funcHashes.begin(), funcHashes.end(), calledHash) == funcHashes.end()) {
              funcHashes.push_back(calledHash);
              *dumpFile << calledHash << " [fillcolor=cyan, style=\"filled\", label=\" " << calledFuncName << " \" ];\n";
            }
            
            builder.SetInsertPoint(call);

            Value *callerHashValue = ConstantInt::get(builder.getInt64Ty(), callerHash);
            Value *calleeHashValue = ConstantInt::get(builder.getInt64Ty(), calledHash);

            Value *args[] = {callerHashValue, calleeHashValue};

            builder.CreateCall(collectData, args);
          }

          if (auto *ret = dyn_cast<ReturnInst>(&I)) {
            if (callerName == "main") {
              builder.SetInsertPoint(ret);

              Value *voidType = ConstantInt::get(builder.getVoidTy(), 0);

              Value *args[] = {voidType};

              builder.CreateCall(writeToFile, args);
            }
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