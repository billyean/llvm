//
// Created by haibo on 7/23/24.
//


#include "AST/AST.h"
#include "Sema/Scope.h"

bool Scope::insertType(AliasTypeDeclaration *Type) {
  return Types.insert(std::pair<StringRef, AliasTypeDeclaration *>(
      Type->getName(), Type))
      .second;
}

bool Scope::insertVariable(VariableDeclaration *Variable) {
  return Variables.insert(std::pair<StringRef, VariableDeclaration *>(
          Variable->getName(), Variable))
      .second;
}

bool Scope::insertFunction(FunctionDeclaration *Function) {
  return Functions.insert(std::pair<StringRef, FunctionDeclaration *>(
          Function->getName(), Function))
      .second;
}

AliasTypeDeclaration *Scope::lookupType(StringRef Name) {
  Scope *S = this;
  while (S) {
    StringMap<AliasTypeDeclaration *>::const_iterator I =
        S->Types.find(Name);
    if (I != S->Types.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;
}

VariableDeclaration *Scope::lookupVariable(StringRef Name) {
  Scope *S = this;
  while (S) {
    StringMap<VariableDeclaration *>::const_iterator I =
        S->Variables.find(Name);
    if (I != S->Variables.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;
}

FunctionDeclaration *Scope::lookupFunction(StringRef Name) {
  Scope *S = this;
  while (S) {
    StringMap<FunctionDeclaration *>::const_iterator I =
        S->Functions.find(Name);
    if (I != S->Functions.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;
}