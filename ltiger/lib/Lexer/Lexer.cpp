//
// Created by haibo on 7/19/24.
//


#include "Lexer/Lexer.h"

tok::TokenKind KeywordFilter::getKeyword(llvm::StringRef Name, tok::TokenKind DefaultTokenCode) {
  auto result = HashTable.find(Name);
  if (result == HashTable.end()) {
    return DefaultTokenCode;
  }
  return result->second;
}

void KeywordFilter::addKeyword(llvm::StringRef Keyword, tok::TokenKind TokenCode) {
  HashTable.insert(std::make_pair(Keyword, TokenCode));
}

void KeywordFilter::addKeywords() {
#define KEYWORD(NAME, FLAGS) addKeyword(StringRef(#NAME), tok::kw_##NAME);
#include "Basic/TokenKinds.def"
}

namespace charinfo {
  LLVM_READNONE inline bool isASCII(char Ch) {
    return static_cast<unsigned char>(Ch) <= 127;
  }

  LLVM_READNONE inline bool isVerticalWhitespace(char Ch) {
    return isASCII(Ch) && (Ch == '\r' || Ch == '\n');
  }

  LLVM_READNONE inline bool isHorizontalWhitespace(char Ch) {
    return isASCII(Ch) && (Ch == ' ' || Ch == '\t' ||
                           Ch == '\f' || Ch == '\v');
  }

  LLVM_READNONE inline bool isWhitespace(char Ch) {
    return isHorizontalWhitespace(Ch) ||
           isVerticalWhitespace(Ch);
  }

  LLVM_READNONE inline bool isDigit(char Ch, bool hasDot = false) {
    if (hasDot) {
      return isASCII(Ch) && Ch >= '0' && Ch <= '9';
    } else {
      return isASCII(Ch) && ((Ch >= '0' && Ch <= '9') || Ch == '.');
    }
  }

  LLVM_READNONE inline bool isHexDigit(char Ch) {
    return isASCII(Ch) &&
           (isDigit(Ch) || (Ch >= 'A' && Ch <= 'F'));
  }

  LLVM_READNONE inline bool isIdentifierHead(char Ch) {
    return isASCII(Ch) &&
           (Ch == '_' || (Ch >= 'A' && Ch <= 'Z') ||
            (Ch >= 'a' && Ch <= 'z'));
  }

  LLVM_READNONE inline bool isIdentifierBody(char Ch) {
    return isIdentifierHead(Ch) || isDigit(Ch);
  }
}

void Lexer::identifier(Token &Result) {
  const char *Start = CurPtr;
  const char *End = CurPtr + 1;
  while (charinfo::isIdentifierBody(*End)) {
    ++End;
  }
  StringRef Name(Start, End - Start);
  tok::TokenKind kind = Filter.getKeyword(Name, tok::identifier);
  formToken(Result, End, kind);
}

void Lexer::number(Token &Result) {
  const char *Start = CurPtr;
  const char *End = Start + 1;
  bool hasDot = false;
  while (charinfo::isDigit(*End, hasDot)) {
    if (*End == '.') {
      hasDot = true;
    }
    ++End;
  }
  if (hasDot) {
    formToken(Result, End, tok::float_lit);
  } else {
    formToken(Result, End, tok::int_lit);
  }
}

void Lexer::comment() {
  const char *End = CurPtr + 2;
  while (*End) {

    // Check for end of comment
    if (*End == '*' && *(End + 1) == '/') {
      End += 2;
    } else {
      ++End;
    }
  }
  if (!*End) {
    Diags.report(getLoc(),
                 diag::err_unterminated_block_comment);
  }
  CurPtr = End;
}

void Lexer::next(Token &token) {
  while (*CurPtr && charinfo::isWhitespace(*CurPtr)) {
    ++CurPtr;
  }
  if (!*CurPtr) {
    token.Kind = tok::eof;
    return;
  }
  if (charinfo::isIdentifierHead(*CurPtr)) {
    identifier(token);
    return;
  } else if (charinfo::isDigit(*CurPtr)) {
    number(token);
    return;
  } else if (*CurPtr == '"' || *CurPtr == '\'') {
    string(token);
    return;
  } else {
    switch (*CurPtr) {
      case '<':
        if (*(CurPtr + 1) == '=') {
          formToken(token, CurPtr + 2, tok::lesseq);
        } else {
          formToken(token, CurPtr + 1, tok::less);
        }
        break;
      case '>':
        if (*(CurPtr + 1) == '=') {
          formToken(token, CurPtr + 2, tok::greateq);
        } else {
          formToken(token, CurPtr + 1, tok::great);
        }
        break;
      case ':':
        if (*(CurPtr + 1) == '=') {
          formToken(token, CurPtr + 2, tok::assign);
        } else {
          formToken(token, CurPtr + 1, tok::colon);
        }
        break;
      case '/':
        if (*(CurPtr + 1) == '*') {
          comment();
          next(token);
        } else
          formToken(token, CurPtr + 1, tok::div);
        break;
      case '*':
        if (*(CurPtr + 1) == '*') {
          formToken(token, CurPtr + 2, tok::pow);
        } else {
          formToken(token, CurPtr + 1, tok::mult);
        }
        break;
      case '=':
        if (*(CurPtr + 1) == '=') {
          formToken(token, CurPtr + 2, tok::equal);
        } else {
          formToken(token, CurPtr + 1, tok::tassign);
        }
        break;
      case '!':
        if (*(CurPtr + 1) == '=') {
          formToken(token, CurPtr + 2, tok::nequal);
        }
        break;
#define CASE(ch, tok) case ch: formToken(token, CurPtr + 1, tok); break
      CASE(',', tok::comma);
      CASE('.', tok::dot);
      CASE(';', tok::semicolon);
      CASE('(', tok::openparen);
      CASE(')', tok::closeparen);
      CASE('[', tok::openbrack);
      CASE(']', tok::closebrack);
      CASE('{', tok::opencurly);
      CASE('}', tok::closecurly);
      CASE('+', tok::plus);
      CASE('-', tok::minus);
      CASE('&', tok::AND);
      CASE('|', tok::OR);
#undef CASE
      default:
        formToken(token, CurPtr + 1, tok::unknown);
    }
    return;
  }
}

void Lexer::formToken(Token &Result, const char *TokEnd, tok::TokenKind Kind) {
  Result.Kind = Kind;
  Result.Length =TokEnd - CurPtr;
  Result.Ptr = CurPtr;
  CurPtr = TokEnd;
}