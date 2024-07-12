//
// Created by haibo on 7/11/24.
//

#ifndef LDU_TOKEN_H
#define LDU_TOKEN_H

#include <cstddef>
#include "Basic/TokenKinds.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

class Token {
  friend class Lexer;
  const char *Ptr;
  size_t Length;
  tok::TokenKind Kind;
public:
  tok::TokenKind getKind() const { return Kind; }

  void setKind(tok::TokenKind K) {  Kind = K; }

  size_t getLength() const { return Length; };

  SMLoc getLocation() const {
    return SMLoc::getFromPointer(Ptr);
  }

  bool is(tok::TokenKind K) const { return K == Kind; }

  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2) const {
    return is(K1) || is(K2);
  }

  template<typename... Ts>
  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  const char *getName() const {
    return tok::getTokenName(Kind);
  }

  StringRef getIdentifier() {
    assert(is(tok::identifier) && "Can't get identifier of non-identifier");
    return StringRef(Ptr, Length);
  }

  StringRef getLiteralData() {
    assert(isOneOf(tok::integer_literal, tok::string_literal) && "Can't get literal data of non-literal");
    return StringRef(Ptr, Length);
  }

  StringRef getText() {
    return StringRef(Ptr, Length);
  }
};
#endif //LDU_TOKEN_H
