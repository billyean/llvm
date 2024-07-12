//
// Created by haibo on 7/11/24.
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

  LLVM_READNONE inline bool isDigit(char Ch) {
    return isASCII(Ch) && Ch >= '0' && Ch <= '9';
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
  while (charinfo::isDigit(*End)) {
    ++End;
  }
  formToken(Result, End, tok::integer_literal);
}

void Lexer::string(Token &Result) {
  const char *Start = CurPtr;
  const char *End = Start + 1;
  while (*End && *End != *Start &&
         !charinfo::isVerticalWhitespace(*End)) {
    ++End;
  }
  if (charinfo::isVerticalWhitespace(*End)) {
  Diags.report(getLoc(),
      diag::err_unterminated_char_or_string);
  }
  formToken(Result, End + 1, tok::string_literal);
}

void Lexer::comment() {
  const char *End = CurPtr + 2;
  unsigned Level = 1;
  while (*End && Level) {
    // Check for nested comment.
    if (*End == '(' && *(End + 1) == '*') {
      End += 2;
      Level++;
    }
      // Check for end of comment
    else if (*End == '*' && *(End + 1) == ')') {
      End += 2;
      Level--;
    } else
      ++End;
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
        if (*(CurPtr + 1) == '=' ) {
          formToken(token, CurPtr + 2,tok::less_equal);
        } else {
          formToken(token, CurPtr + 1, tok::less);
        }
        break;
      case '>':
        if (*(CurPtr + 1) == '=' ) {
          formToken(token, CurPtr + 2,tok::greater_equal);
        } else {
          formToken(token, CurPtr + 1, tok::greater);
        }
        break;
      case ':':
        if (*(CurPtr + 1) == '=' ) {
          formToken(token, CurPtr + 2,tok::assign);
        } else {
          formToken(token, CurPtr + 1, tok::unknown);
        }
        break;
      case '(':
        if (*(CurPtr + 1) == '*') {
          comment();
          next(token);
        } else
          formToken(token, CurPtr + 1, tok::l_paren);
        break;
#define CASE(ch, tok) case ch: formToken(token, CurPtr + 1, tok); break
      CASE('+', tok::plus);
      CASE('-', tok::minus);
      CASE('*', tok::star);
      CASE('/', tok::slash);
      CASE(')', tok::r_paren);
      CASE(',', tok::comma);
      CASE(';', tok::semicolon);
      CASE('=', tok::equal_to);
      CASE('#', tok::not_equal);
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