//
// Created by Haibo Yan on 7/10/24.
//

#ifndef LDING_PARSER_H
#define LDING_PARSER_H

#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"
#include "AST.h"

class Parser {
    Lexer &Lex;
    Token Tok;
    bool HashError;
    void error() {
        llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
        HashError = true;
    }

    void advance() {
        Lex.next(Tok);
    }

    bool expect(Token::TokenKind Kind) {
        if (Tok.is(Kind)) {
            return false;
        }
        error();
        return true;
    }

    bool consume(Token::TokenKind Kind) {
        if (expect(Kind)) {
            return true;
        }
        advance();
        return false;
    }

    AST *parseCalc();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();
public:
  Parser(Lexer &Lex): Lex(Lex), HashError(false) {
    advance();
  }

  bool hashError() {
    return HashError;
  }

  AST *parse();
};


#endif //LDING_PARSER_H
