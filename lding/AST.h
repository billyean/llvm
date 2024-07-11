//
// Created by Haibo Yan on 7/10/24.
//

#ifndef LDING_AST_H
#define LDING_AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;
class ASTVisitor {
public:
    virtual void visit(AST &){};
    virtual void visit(Expr &){};
    virtual void visit(Factor &) = 0;
    virtual void visit(BinaryOp &) = 0;
    virtual void visit(WithDecl &) = 0;
};

class AST {
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor &V) = 0;
};

class Expr: public AST {
public:
    Expr() {}
};

class Factor: public Expr {
public:
    enum ValueKind { Ident, Number };
private:
    ValueKind Kind;
    llvm::StringRef Val;

public:
    Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val){}

    ValueKind getKind() { return Kind; }

    llvm::StringRef getVal() { return Val; }

    void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};


class BinaryOp: public Expr {
public:
    enum Operator { Plus, Minus, Mul, Div};
private:
    Expr *Left;
    Expr *Right;
    Operator Op;
public:
    BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}

    Expr *getLeft() { return Left; }

    Expr *getRight() { return Right; }

    Operator getOperator() { return Op; }

    void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};


class WithDecl: public Expr {
public:
    using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
    VarVector Vars;
    Expr *E;
public:
    WithDecl(VarVector Vars, Expr *E) : Vars(std::move(Vars)), E(E) {}

    VarVector::const_iterator begin() { return Vars.begin(); }

    VarVector::const_iterator end() { return Vars.end(); }

    Expr *getExpr() { return E; }

    void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

#endif //LDING_AST_H
