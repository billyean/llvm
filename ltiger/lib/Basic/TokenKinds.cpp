//
// Created by haibo on 7/19/24.
//

#include "Basic/TokenKinds.h"
#include "llvm/Support/ErrorHandling.h"

static const char * const TokNames[] = {
#define TOK(ID) #ID,
#define KEYWORD(ID, FLAG) #ID,
#include "Basic/TokenKinds.def"
    nullptr
};

const char *tok::getTokenName(tok::TokenKind Kind) {
  return TokNames[Kind];
}

const char *tok::getPunctuatorSpelling(tok::TokenKind Kind) {
  switch (Kind) {
#define PUNCTUATOR(ID, SP) case ID: return SP;
#include "Basic/TokenKinds.def"
    default: break;
  }
  return nullptr;
}

const char *tok::getKeywordSpelling(tok::TokenKind Kind) {
  switch (Kind) {
#define KEYWORD(ID, FLAG) case kw_ ## ID: return #ID;
#include "Basic/TokenKinds.def"
    default: break;
  }
  return nullptr;
}
