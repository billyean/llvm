//
// Created by haibo on 7/12/24.
//

#ifndef LDU_LDUPROCEDURE_H
#define LDU_LDUPROCEDURE_H

#include "AST/AST.h"
#include "CodeGen/LDUModule.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

class LDUProcedure {
  LDUModule &LDUM;
  llvm::IRBuilder<> Builder;

  llvm::BasicBlock *Curr;

  ProcedureDeclaration *Proc;
  llvm::FunctionType *Fty;
  llvm::Function *Fn;

  struct BasicBlockDef {
    // Maps the variable (or formal parameter) to its
    // definition.
    llvm::DenseMap<Decl *, llvm::TrackingVH<llvm::Value>> Defs;
    // Set of incompleted phi instructions.
    llvm::DenseMap<llvm::PHINode *, Decl *> IncompletePhis;
    // Block is sealed, that is, no more predecessors will
    // be added.
    unsigned Sealed: 1;

    BasicBlockDef() : Sealed(0) {}
  };

  llvm::DenseMap<llvm::BasicBlock *, BasicBlockDef>
      CurrentDef;

  void writeLocalVariable(llvm::BasicBlock *BB, Decl *Decl,
                          llvm::Value *Val);

  llvm::Value *readLocalVariable(llvm::BasicBlock *BB,
                                 Decl *Decl);

  llvm::Value *
  readLocalVariableRecursive(llvm::BasicBlock *BB,
                             Decl *Decl);

  llvm::PHINode *addEmptyPhi(llvm::BasicBlock *BB,
                             Decl *Decl);

  llvm::Value *addPhiOperands(llvm::BasicBlock *BB,
                              Decl *Decl,
                              llvm::PHINode *Phi);

  llvm::Value *optimizePhi(llvm::PHINode *Phi);

  void sealBlock(llvm::BasicBlock *BB);

  llvm::DenseMap<FormalParameterDeclaration *,
      llvm::Argument *>
      FormalParams;

  void writeVariable(llvm::BasicBlock *BB, Decl *Decl,
                     llvm::Value *Val);

  llvm::Value *readVariable(llvm::BasicBlock *BB,
                            Decl *Decl);

  llvm::Type *mapType(Decl *Decl,
                      bool HonorReference = true);

  llvm::FunctionType *
  createFunctionType(ProcedureDeclaration *Proc);

  llvm::Function *createFunction(ProcedureDeclaration *Proc,
                                 llvm::FunctionType *FTy);

protected:
  void setCurr(llvm::BasicBlock *BB) {
    Curr = BB;
    Builder.SetInsertPoint(Curr);
  }

  llvm::Value *emitInfixExpr(InfixExpression *E);

  llvm::Value *emitPrefixExpr(PrefixExpression *E);

  llvm::Value *emitExpr(Expr *E);

  void emitStmt(AssignmentStatement *Stmt);

  void emitStmt(ProcedureCallStatement *Stmt);

  void emitStmt(IfStatement *Stmt);

  void emitStmt(WhileStatement *Stmt);

  void emitStmt(ReturnStatement *Stmt);

  void emit(const StmtList &Stmts);

public:
  LDUProcedure(LDUModule &LDUM)
      : LDUM(LDUM), Builder(LDUM.getLLVMContext()),
        Curr(nullptr) {};

  void run(ProcedureDeclaration *Proc);
};

#endif //LDU_LDUPROCEDURE_H
