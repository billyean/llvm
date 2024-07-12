//
// Created by haibo on 7/12/24.
//
#include "CodeGen/LDUModule.h"
#include "CodeGen/LDUProcedure.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"

void LDUModule::init() {
  VoidType = llvm::Type::getVoidTy(getLLVMContext());
  Int1Type = llvm::Type::getInt1Ty(getLLVMContext());
  Int32Type = llvm::Type::getInt32Ty(getLLVMContext());
  Int64Type = llvm::Type::getInt64Ty(getLLVMContext());
  Int32Zero =
      llvm::ConstantInt::get(Int32Type, 0, /*isSigned*/ true);
}

llvm::Type *LDUModule::convertType(TypeDeclaration *Ty) {
  if (Ty->getName() == "INTEGER")
    return Int64Type;
  if (Ty->getName() == "BOOLEAN")
    return Int1Type;
  llvm::report_fatal_error("Unsupported type");
}

std::string LDUModule::mangleName(Decl *D) {
  std::string Mangled("_t");
  llvm::SmallVector<llvm::StringRef, 4> Parts;
  for (; D; D = D->getEnclosingDecl())
    Parts.push_back(D->getName());
  while (!Parts.empty()) {
    llvm::StringRef Name = Parts.pop_back_val();
    Mangled.append(
        llvm::Twine(Name.size()).concat(Name).str());
  }
  return Mangled;
}

llvm::GlobalObject *LDUModule::getGlobal(Decl *D) {
  return Globals[D];
}

void LDUModule::run(ModuleDeclaration *Mod) {
  for (auto *Decl : Mod->getDecls()) {
    if (auto *Var =
        llvm::dyn_cast<VariableDeclaration>(Decl)) {
      // Create global variables
      auto *V = new llvm::GlobalVariable(
          *M, convertType(Var->getType()),
          /*isConstant=*/false,
          llvm::GlobalValue::PrivateLinkage, nullptr,
          mangleName(Var));
      Globals[Var] = V;
    } else if (auto *Proc =
        llvm::dyn_cast<ProcedureDeclaration>(
            Decl)) {
      LDUProcedure CGP(*this);
      CGP.run(Proc);
    }
  }
}