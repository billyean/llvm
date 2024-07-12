//
// Created by haibo on 7/12/24.
//
#include "CodeGen/LDUProcedure.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/Casting.h"

void LDUProcedure::writeLocalVariable(llvm::BasicBlock *BB,
                                     Decl *Decl,
                                     llvm::Value *Val) {
  assert(BB && "Basic block is nullptr");
  assert(
      (llvm::isa<VariableDeclaration>(Decl) ||
       llvm::isa<FormalParameterDeclaration>(Decl)) &&
      "Declaration must be variable or formal parameter");
  assert(Val && "Value is nullptr");
  CurrentDef[BB].Defs[Decl] = Val;
}

llvm::Value *
LDUProcedure::readLocalVariable(llvm::BasicBlock *BB,
                               Decl *Decl) {
  assert(BB && "Basic block is nullptr");
  assert(
      (llvm::isa<VariableDeclaration>(Decl) ||
       llvm::isa<FormalParameterDeclaration>(Decl)) &&
      "Declaration must be variable or formal parameter");
  auto Val = CurrentDef[BB].Defs.find(Decl);
  if (Val != CurrentDef[BB].Defs.end())
    return Val->second;
  return readLocalVariableRecursive(BB, Decl);
}

llvm::Value *LDUProcedure::readLocalVariableRecursive(
    llvm::BasicBlock *BB, Decl *Decl) {
  llvm::Value *Val = nullptr;
  if (!CurrentDef[BB].Sealed) {
    // Add incomplete phi for variable.
    llvm::PHINode *Phi = addEmptyPhi(BB, Decl);
    CurrentDef[BB].IncompletePhis[Phi] = Decl;
    Val = Phi;
  } else if (auto *PredBB = BB->getSinglePredecessor()) {
    // Only one predecessor.
    Val = readLocalVariable(PredBB, Decl);
  } else {
    // Create empty phi instruction to break potential
    // cycles.
    llvm::PHINode *Phi = addEmptyPhi(BB, Decl);
    writeLocalVariable(BB, Decl, Phi);
    Val = addPhiOperands(BB, Decl, Phi);
  }
  writeLocalVariable(BB, Decl, Val);
  return Val;
}

llvm::PHINode *
LDUProcedure::addEmptyPhi(llvm::BasicBlock *BB, Decl *Decl) {
  return BB->empty()
         ? llvm::PHINode::Create(mapType(Decl), 0, "",
                                 BB)
         : llvm::PHINode::Create(mapType(Decl), 0, "",
                                 &BB->front());
}

llvm::Value *LDUProcedure::addPhiOperands(
    llvm::BasicBlock *BB, Decl *Decl, llvm::PHINode *Phi) {
  for (auto *PredBB : llvm::predecessors(BB))
    Phi->addIncoming(readLocalVariable(PredBB, Decl),
                     PredBB);
  return optimizePhi(Phi);
}

llvm::Value *LDUProcedure::optimizePhi(llvm::PHINode *Phi) {
  llvm::Value *Same = nullptr;
  for (llvm::Value *V : Phi->incoming_values()) {
    if (V == Same || V == Phi)
      continue;
    if (Same && V != Same)
      return Phi;
    Same = V;
  }
  if (Same == nullptr)
    Same = llvm::UndefValue::get(Phi->getType());
  // Collect phi instructions using this one.
  llvm::SmallVector<llvm::PHINode *, 8> CandidatePhis;
  for (llvm::Use &U : Phi->uses()) {
    if (auto *P =
        llvm::dyn_cast<llvm::PHINode>(U.getUser()))
      if (P != Phi)
        CandidatePhis.push_back(P);
  }
  Phi->replaceAllUsesWith(Same);
  Phi->eraseFromParent();
  for (auto *P : CandidatePhis)
    optimizePhi(P);
  return Same;
}

void LDUProcedure::sealBlock(llvm::BasicBlock *BB) {
  assert(!CurrentDef[BB].Sealed &&
         "Attempt to seal already sealed block");
  for (auto PhiDecl : CurrentDef[BB].IncompletePhis) {
    addPhiOperands(BB, PhiDecl.second, PhiDecl.first);
  }
  CurrentDef[BB].IncompletePhis.clear();
  CurrentDef[BB].Sealed = true;
}

void LDUProcedure::writeVariable(llvm::BasicBlock *BB,
                                Decl *D, llvm::Value *Val) {
  if (auto *V = llvm::dyn_cast<VariableDeclaration>(D)) {
    if (V->getEnclosingDecl() == Proc)
      writeLocalVariable(BB, D, Val);
    else if (V->getEnclosingDecl() ==
        LDUM.getModuleDecl()) {
      Builder.CreateStore(Val, LDUM.getGlobal(D));
    } else
      llvm::report_fatal_error(
          "Nested procedures not yet supported");
  } else if (auto *FP =
      llvm::dyn_cast<FormalParameterDeclaration>(
          D)) {
    if (FP->isVar()) {
      Builder.CreateStore(Val, FormalParams[FP]);
    } else
      writeLocalVariable(BB, D, Val);
  } else
    llvm::report_fatal_error("Unsupported declaration");
}

llvm::Value *LDUProcedure::readVariable(llvm::BasicBlock *BB,
                                       Decl *D) {
  if (auto *V = llvm::dyn_cast<VariableDeclaration>(D)) {
    if (V->getEnclosingDecl() == Proc)
      return readLocalVariable(BB, D);
    else if (V->getEnclosingDecl() ==
             LDUM.getModuleDecl()) {
      return Builder.CreateLoad(mapType(D),
                                LDUM.getGlobal(D));
    } else
      llvm::report_fatal_error(
          "Nested procedures not yet supported");
  } else if (auto *FP =
      llvm::dyn_cast<FormalParameterDeclaration>(
          D)) {
    if (FP->isVar()) {
      return Builder.CreateLoad(mapType(FP, false),
                                FormalParams[FP]);
    } else
      return readLocalVariable(BB, D);
  } else
    llvm::report_fatal_error("Unsupported declaration");
}

llvm::Type *LDUProcedure::mapType(Decl *Decl,
                                 bool HonorReference) {
  if (auto *FP = llvm::dyn_cast<FormalParameterDeclaration>(
      Decl)) {
    if (FP->isVar() && HonorReference)
      return llvm::PointerType::get(LDUM.getLLVMContext(),
          /*AddressSpace=*/0);
    return LDUM.convertType(FP->getType());
  }
  if (auto *V = llvm::dyn_cast<VariableDeclaration>(Decl))
    return LDUM.convertType(V->getType());
  return LDUM.convertType(llvm::cast<TypeDeclaration>(Decl));
}

llvm::FunctionType *LDUProcedure::createFunctionType(
    ProcedureDeclaration *Proc) {
  llvm::Type *ResultTy = LDUM.VoidType;
  if (Proc->getRetType()) {
    ResultTy = mapType(Proc->getRetType());
  }
  auto FormalParams = Proc->getFormalParams();
  llvm::SmallVector<llvm::Type *, 8> ParamTypes;
  for (auto FP : FormalParams) {
    llvm::Type *Ty = mapType(FP);
    ParamTypes.push_back(Ty);
  }
  return llvm::FunctionType::get(ResultTy, ParamTypes,
      /*IsVarArgs=*/false);
}

llvm::Function *
LDUProcedure::createFunction(ProcedureDeclaration *Proc,
                            llvm::FunctionType *FTy) {
  llvm::Function *Fn = llvm::Function::Create(
      Fty, llvm::GlobalValue::ExternalLinkage,
      LDUM.mangleName(Proc), LDUM.getModule());
  // Give parameters a name.
  for (auto Pair : llvm::enumerate(Fn->args())) {
    llvm::Argument &Arg = Pair.value();
    FormalParameterDeclaration *FP =
        Proc->getFormalParams()[Pair.index()];
    if (FP->isVar()) {
      llvm::AttrBuilder Attr(LDUM.getLLVMContext());
      llvm::TypeSize Sz =
          LDUM.getModule()->getDataLayout().getTypeStoreSize(
              LDUM.convertType(FP->getType()));
      Attr.addDereferenceableAttr(Sz);
      Attr.addAttribute(llvm::Attribute::NoCapture);
      Arg.addAttrs(Attr);
    }
    Arg.setName(FP->getName());
  }
  return Fn;
}

llvm::Value *
LDUProcedure::emitInfixExpr(InfixExpression *E) {
  llvm::Value *Left = emitExpr(E->getLeft());
  llvm::Value *Right = emitExpr(E->getRight());
  llvm::Value *Result = nullptr;
  switch (E->getOperatorInfo().getKind()) {
    case tok::plus:
      Result = Builder.CreateNSWAdd(Left, Right);
      break;
    case tok::minus:
      Result = Builder.CreateNSWSub(Left, Right);
      break;
    case tok::star:
      Result = Builder.CreateNSWMul(Left, Right);
      break;
    case tok::kw_DIV:
      Result = Builder.CreateSDiv(Left, Right);
      break;
    case tok::kw_MOD:
      Result = Builder.CreateSRem(Left, Right);
      break;
    case tok::equal_to:
      Result = Builder.CreateICmpEQ(Left, Right);
      break;
    case tok::not_equal:
      Result = Builder.CreateICmpNE(Left, Right);
      break;
    case tok::less:
      Result = Builder.CreateICmpSLT(Left, Right);
      break;
    case tok::less_equal:
      Result = Builder.CreateICmpSLE(Left, Right);
      break;
    case tok::greater:
      Result = Builder.CreateICmpSGT(Left, Right);
      break;
    case tok::greater_equal:
      Result = Builder.CreateICmpSGE(Left, Right);
      break;
    case tok::kw_AND:
      Result = Builder.CreateAnd(Left, Right);
      break;
    case tok::kw_OR:
      Result = Builder.CreateOr(Left, Right);
      break;
    case tok::slash:
      // Divide by real numbers not supported.
      LLVM_FALLTHROUGH;
    default:
      llvm_unreachable("Wrong operator");
  }
  return Result;
}

llvm::Value *
LDUProcedure::emitPrefixExpr(PrefixExpression *E) {
  llvm::Value *Result = emitExpr(E->getExpr());
  switch (E->getOperatorInfo().getKind()) {
    case tok::plus:
      // Identity - nothing to do.
      break;
    case tok::minus:
      Result = Builder.CreateNeg(Result);
      break;
    case tok::kw_NOT:
      Result = Builder.CreateNot(Result);
      break;
    default:
      llvm_unreachable("Wrong operator");
  }
  return Result;
}

llvm::Value *LDUProcedure::emitExpr(Expr *E) {
  if (auto *Infix = llvm::dyn_cast<InfixExpression>(E)) {
    return emitInfixExpr(Infix);
  } else if (auto *Prefix =
      llvm::dyn_cast<PrefixExpression>(E)) {
    return emitPrefixExpr(Prefix);
  } else if (auto *Var =
      llvm::dyn_cast<VariableAccess>(E)) {
    auto *Decl = Var->getDecl();
    // With more languages features in place, here you need
    // to add array and record support.
    return readVariable(Curr, Decl);
  } else if (auto *Const =
      llvm::dyn_cast<ConstantAccess>(E)) {
    return emitExpr(Const->geDecl()->getExpr());
  } else if (auto *IntLit =
      llvm::dyn_cast<IntegerLiteral>(E)) {
    return llvm::ConstantInt::get(LDUM.Int64Type,
                                  IntLit->getValue());
  } else if (auto *BoolLit =
      llvm::dyn_cast<BooleanLiteral>(E)) {
    return llvm::ConstantInt::get(LDUM.Int1Type,
                                  BoolLit->getValue());
  }
  llvm::report_fatal_error("Unsupported expression");
}

void LDUProcedure::emitStmt(AssignmentStatement *Stmt) {
  auto *Val = emitExpr(Stmt->getExpr());
  writeVariable(Curr, Stmt->getVar(), Val);
}

void LDUProcedure::emitStmt(ProcedureCallStatement *Stmt) {
  llvm::report_fatal_error("not implemented");
}

void LDUProcedure::emitStmt(IfStatement *Stmt) {
  bool HasElse = Stmt->getElseStmts().size() > 0;

  // Create the required basic blocks.
  llvm::BasicBlock *IfBB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "if.body", Fn);
  llvm::BasicBlock *ElseBB =
      HasElse ? llvm::BasicBlock::Create(LDUM.getLLVMContext(),
                                         "else.body", Fn)
              : nullptr;
  llvm::BasicBlock *AfterIfBB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "after.if", Fn);

  llvm::Value *Cond = emitExpr(Stmt->getCond());
  Builder.CreateCondBr(Cond, IfBB,
                       HasElse ? ElseBB : AfterIfBB);
  sealBlock(Curr);

  setCurr(IfBB);
  emit(Stmt->getIfStmts());
  if (!Curr->getTerminator()) {
    Builder.CreateBr(AfterIfBB);
  }
  sealBlock(Curr);

  if (HasElse) {
    setCurr(ElseBB);
    emit(Stmt->getElseStmts());
    if (!Curr->getTerminator()) {
      Builder.CreateBr(AfterIfBB);
    }
    sealBlock(Curr);
  }
  setCurr(AfterIfBB);
}

void LDUProcedure::emitStmt(WhileStatement *Stmt) {
  // The basic block for the condition.
  llvm::BasicBlock *WhileCondBB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "while.cond", Fn);
  // The basic block for the while body.
  llvm::BasicBlock *WhileBodyBB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "while.body", Fn);
  // The basic block after the while statement.
  llvm::BasicBlock *AfterWhileBB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "after.while", Fn);

  Builder.CreateBr(WhileCondBB);
  sealBlock(Curr);
  setCurr(WhileCondBB);
  llvm::Value *Cond = emitExpr(Stmt->getCond());
  Builder.CreateCondBr(Cond, WhileBodyBB, AfterWhileBB);

  setCurr(WhileBodyBB);
  emit(Stmt->getWhileStmts());
  Builder.CreateBr(WhileCondBB);
  sealBlock(WhileCondBB);
  sealBlock(Curr);

  setCurr(AfterWhileBB);
}

void LDUProcedure::emitStmt(ReturnStatement *Stmt) {
  if (Stmt->getRetVal()) {
    llvm::Value *RetVal = emitExpr(Stmt->getRetVal());
    Builder.CreateRet(RetVal);
  } else {
    Builder.CreateRetVoid();
  }
}

void LDUProcedure::emit(const StmtList &Stmts) {
  for (auto *S : Stmts) {
    if (auto *Stmt = llvm::dyn_cast<AssignmentStatement>(S))
      emitStmt(Stmt);
    else if (auto *Stmt =
        llvm::dyn_cast<ProcedureCallStatement>(S))
      emitStmt(Stmt);
    else if (auto *Stmt = llvm::dyn_cast<IfStatement>(S))
      emitStmt(Stmt);
    else if (auto *Stmt = llvm::dyn_cast<WhileStatement>(S))
      emitStmt(Stmt);
    else if (auto *Stmt =
        llvm::dyn_cast<ReturnStatement>(S))
      emitStmt(Stmt);
    else
      llvm_unreachable("Unknown statement");
  }
}

void LDUProcedure::run(ProcedureDeclaration *Proc) {
  this->Proc = Proc;
  Fty = createFunctionType(Proc);
  Fn = createFunction(Proc, Fty);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(
      LDUM.getLLVMContext(), "entry", Fn);
  setCurr(BB);

  for (auto Pair : llvm::enumerate(Fn->args())) {
    llvm::Argument *Arg = &Pair.value();
    FormalParameterDeclaration *FP =
        Proc->getFormalParams()[Pair.index()];
    // Create mapping FormalParameter -> llvm::Argument for
    // VAR parameters.
    FormalParams[FP] = Arg;
    writeLocalVariable(Curr, FP, Arg);
  }

  for (auto *D : Proc->getDecls()) {
    if (auto *Var =
        llvm::dyn_cast<VariableDeclaration>(D)) {
      llvm::Type *Ty = mapType(Var);
      if (Ty->isAggregateType()) {
        llvm::Value *Val = Builder.CreateAlloca(Ty);
        writeLocalVariable(Curr, Var, Val);
      }
    }
  }

  auto Block = Proc->getStmts();
  emit(Proc->getStmts());
  if (!Curr->getTerminator()) {
    Builder.CreateRetVoid();
  }
  sealBlock(Curr);
}