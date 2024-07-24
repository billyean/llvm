//
// Created by haibo on 7/23/24.
//

#ifndef LTIGER_SCOPE_H
#define LTIGER_SCOPE_H

#include "Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

class AliasTypeDeclaration;

class VariableDeclaration;

class FunctionDeclaration;

class Scope {
  Scope *Parent;
  StringMap<AliasTypeDeclaration *> Types;
  StringMap<VariableDeclaration *> Variables;
  StringMap<FunctionDeclaration *> Functions;

public:
  explicit Scope(Scope *Parent = nullptr) : Parent(Parent) {}

  bool insertType(AliasTypeDeclaration *Type);

  AliasTypeDeclaration *lookupType(StringRef Name);

  bool insertVariable(VariableDeclaration *Variable);

  VariableDeclaration *lookupVariable(StringRef Name);

  bool insertFunction(FunctionDeclaration *Function);

  FunctionDeclaration *lookupFunction(StringRef Name);

  Scope *getParent() { return Parent; }
};

#endif //LTIGER_SCOPE_H
