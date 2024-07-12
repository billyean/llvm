//
// Created by haibo on 7/12/24.
//
#include "CodeGen/CodeGen.h"
#include "CodeGen/LDUProcedure.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

CodeGen *CodeGen::create(llvm::LLVMContext &Ctx, llvm::TargetMachine *TM) {
  return new CodeGen(Ctx, TM);
}

std::unique_ptr<llvm::Module> CodeGen::run(ModuleDeclaration *CM, std::string FileName) {
  std::unique_ptr<llvm::Module> M =
      std::make_unique<llvm::Module>(FileName, Ctx);
  M->setTargetTriple(TM->getTargetTriple().getTriple());
  M->setDataLayout(TM->createDataLayout());
  LDUModule LDUM(M.get());
  LDUM.run(CM);
  return M;
}