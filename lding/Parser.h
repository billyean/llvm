//
// Created by Haibo Yan on 7/10/24.
//

#ifndef LDING_PARSER_H
#define LDING_PARSER_H

#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"

class Parser {
    Lexer &lexer;
    Token Tok;
    bool HashError;
    void error() {
        llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
        HashError = true;
    }

    void advance() {
        lexer.next(Tok);
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
};


#endif //LDING_PARSER_H
