//
// Created by Haibo Yan on 7/10/24.
//

#ifndef LDING_SEMA_H
#define LDING_SEMA_H
#include "AST.h"
#include "Lexer.h"

class Sema {
public:
    bool semantic(AST *Tree);
};

#endif //LDING_SEMA_H
