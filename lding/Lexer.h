//
// Created by Haibo Yan on 7/10/24.
//

#ifndef LDING_LEXER_H
#define LDING_LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"


using namespace llvm;

class Token {
    friend class Lexer;
public:
    enum TokenKind: unsigned short {
        eoi, unknown, ident, number, comma, colon, plus, minus, star, slash, l_paren, r_paren, KW_with
    };
private:
    TokenKind Kind;
    StringRef Text;
public:
    TokenKind getKind() const {
        return Kind;
    }

    StringRef getText() const {
        return Text;
    }

    bool is(TokenKind K) const { return K == Kind; }

    bool isOneOf(TokenKind K1, TokenKind K2) const {
        return is(K1) || is(K2);
    }

    template<typename... Ts>
    bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
        return is(K1) || isOneOf(K2, Ks...);
    }
};

class Lexer {
    const char *BufferStart;
    const char *BufferPtr;
public:
    Lexer(const StringRef &Buffer) {
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }

    void next(Token &token);
private:
    void formToken(Token &Result, const char *TokEnd, Token::TokenKind Kind);
};

#endif //LDING_LEXER_H
