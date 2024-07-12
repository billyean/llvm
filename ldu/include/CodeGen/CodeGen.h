//
// Created by haibo on 7/11/24.
//

#ifndef LDU_CODEGEN_H
#define LDU_CODEGEN_H

#include "AST/AST.h"
#include "llvm/Target/TargetMachine.h"
#include <string>


class CodeGen {
  llvm::LLVMContext &Ctx;
  llvm::TargetMachine *TM;

protected:
  CodeGen(llvm::LLVMContext &Ctx,
                llvm::TargetMachine *TM)
      : Ctx(Ctx), TM(TM) {}

public:
  static CodeGen *create(llvm::LLVMContext &Ctx,
                               llvm::TargetMachine *TM);

  std::unique_ptr<llvm::Module> run(ModuleDeclaration *CM,
                                    std::string FileName);
};

#endif //LDU_CODEGEN_H
