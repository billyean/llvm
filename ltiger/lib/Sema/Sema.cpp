//
// Created by haibo on 7/23/24.
//
#include "AST/AST.h"
#include "Sema/Sema.h"

void Sema::initialize() {
  // Setup global scope.
  CurrentScope = new Scope();
  CurrentTypeDeclaration = nullptr;
  CurrentVariableDeclaration = nullptr;
  CurrentFunctionDeclaration = nullptr;

  IntegerType =
      new IDTypeDeclaration(CurrentTypeDeclaration, SMLoc(), "INTEGER");
  BooleanType =
      new IDTypeDeclaration(CurrentTypeDeclaration, SMLoc(), "BOOLEAN");
}

Expression *Sema::actOnIntegerLiteral(llvm::SMLoc Loc, llvm::StringRef Literal) {

  llvm::APInt Value(64, Literal, 10);
  return new IntegerLiteral(Loc, Value, IntegerType);
}

Expression *Sema::actOnFloatLiteral(llvm::SMLoc Loc, llvm::StringRef Literal) {
  llvm::APFloat Value(APFloatBase::EnumToSemantics( llvm::APFloatBase::Semantics::S_BFloat), Literal);
  return new FloatLiteral(Loc, Value, IntegerType);
}

void Sema::actOnAliasTypeDeclaration(AliasTypeDeclarationList &AliasTypeDeclarations,
                                     Ident *I, AliasTypeDeclaration *ATD) {
  assert(CurrentScope && "CurrentScope not set");




}
