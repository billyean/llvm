//
// Created by haibo on 7/11/24.
//

#ifndef LDU_LDUMODULE_H
#define LDU_LDUMODULE_H

#include "llvm/IR/Module.h"
#include "llvm/ADT/DenseMap.h"
#include "AST/AST.h"

class LDUModule {
  llvm::Module *M;
  ModuleDeclaration *ModuleDecl;
private:
  llvm::DenseMap<Decl *, llvm::GlobalObject *> Globals;
public:
  llvm::Type *VoidType;
  llvm::Type *Int1Type;
  llvm::Type *Int32Type;
  llvm::Type *Int64Type;
  llvm::Constant *Int32Zero;

  LDUModule(llvm::Module *M) : M(M) {
    init();
  }

  void init();

  llvm::LLVMContext &getLLVMContext() {
    return M->getContext();
  }

  llvm::Module *getModule() {
    return M;
  }

  ModuleDeclaration *getModuleDecl() const {
    return ModuleDecl;
  }

  llvm::Type *convertType(TypeDeclaration *Ty);
  
  std::string mangleName(Decl *D);

  llvm::GlobalObject *getGlobal(Decl *);

  void run(ModuleDeclaration *Mod);
};
#endif //LDU_LDUMODULE_H
