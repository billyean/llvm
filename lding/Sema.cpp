//
// Created by Haibo Yan on 7/10/24.
//

#include <llvm/Support/raw_ostream.h>
#include "Sema.h"
#include "llvm/ADT/StringSet.h"

namespace {
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> Scope;
        bool HashError;
        enum ErrorType { Twice, Not};
        void error(ErrorType ET, llvm::StringRef V) {
            llvm::errs() << "Variable " << V << " "
                << (ET == Twice ? "already" : "not")
                << " declared\n";
            HashError = true;
        }
    public:
        DeclCheck(): HashError(false) {}
        bool hasError() { return HashError; }

        void visit(Factor &Node) override {
            if (Node.getKind() == Factor::Ident) {
                if (Scope.find(Node.getVal()) == Scope.end()) {
                    error(Not, Node.getVal());
                }
            }
        }

        void visit(BinaryOp &Node) override {
            if (Node.getLeft()) {
                Node.getLeft()->accept(*this);
            } else {
                HashError = true;
            }
            if (Node.getRight()) {
                Node.getRight()->accept(*this);
            } else {
                HashError = true;
            }
        }

        void visit(WithDecl &Node) override {
            for (auto I = Node.begin(); I != Node.end(); ++I) {
                if (!Scope.insert(*I).second)
                    error(Twice, *I);
            }
            if (Node.getExpr()) {
                Node.getExpr()->accept(*this);
            } else {
                HashError = true;
            }
        }
    };
}

bool Sema::semantic(AST *Tree) {
    if (!Tree) {
        return false;
    }
    DeclCheck Check;
    Tree->accept(Check);
    return Check.hasError();
}
