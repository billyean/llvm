//
// Created by haibo on 7/11/24.
//

#ifndef LDU_SCOPE_H
#define LDU_SCOPE_H

#include "Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

class Decl;

class Scope {
  Scope *Parent;
  StringMap<Decl *> Symbols;

public:
  Scope(Scope *Parent = nullptr) : Parent(Parent) {}

  bool insert(Decl *Declaration);
  Decl *lookup(StringRef Name);

  Scope *getParent() { return Parent; }
};
#endif //LDU_SCOPE_H
