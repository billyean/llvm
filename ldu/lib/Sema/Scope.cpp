//
// Created by haibo on 7/11/24.
//

#include "Sema/Scope.h"
#include "AST/AST.h"

bool Scope::insert(Decl *Declaration) {
  return Symbols
      .insert(std::pair<StringRef, Decl *>(
          Declaration->getName(), Declaration))
      .second;
}

Decl *Scope::lookup(StringRef Name) {
  Scope *S = this;
  while (S) {
    StringMap<Decl *>::const_iterator I =
        S->Symbols.find(Name);
    if (I != S->Symbols.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;
}
