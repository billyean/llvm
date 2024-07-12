//
// Created by haibo on 7/11/24.
//

#ifndef LDU_LEXER_H
#define LDU_LEXER_H

#include "Lexer/Token.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/SourceMgr.h"
#include "Basic/Diagnostic.h"

class KeywordFilter {
  llvm::StringMap<tok::TokenKind> HashTable;
  void addKeyword(StringRef Keyword, tok::TokenKind TokenCode);
public:
  void addKeywords();

  tok::TokenKind getKeyword(StringRef Name, tok::TokenKind DefaultTokenCode = tok::unknown);
};

class Lexer {
  SourceMgr &SrcMgr;

  DiagnosticsEngine &Diags;

  KeywordFilter Filter;
  const char *CurPtr;

  unsigned int CurBuffer = 0;
  StringRef CurBuf;
public:
  Lexer(SourceMgr &SrcMgr, DiagnosticsEngine &Diags): SrcMgr(SrcMgr), Diags(Diags) {
    CurBuffer = SrcMgr.getMainFileID();
    CurBuf = SrcMgr.getMemoryBuffer(CurBuffer)->getBuffer();
    CurPtr = CurBuf.begin();
    Filter.addKeywords();
  }

  DiagnosticsEngine &getDiagnostics() const {
    return Diags;
  }

  void next(Token &token);
private:
  void identifier(Token &Result);
  void number(Token &Result);
  void string(Token &Result);
  void comment();

  SMLoc getLoc() { return SMLoc::getFromPointer(CurPtr); }

  void formToken(Token &Result, const char *TokEnd, tok::TokenKind Kind);
};

#endif //LDU_LEXER_H
