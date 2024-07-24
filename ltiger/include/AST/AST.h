//
// Created by haibo on 7/19/24.
//

#ifndef LTIGER_AST_H
#define LTIGER_AST_H

#include "Basic/LLVM.h"
#include "Basic/TokenKinds.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include <string>
#include <utility>
#include <vector>

class Declaration;

class AliasTypeDeclaration;

class VariableDeclaration;

class FunctionDeclaration;

class Expression;

class Statement;

class FormalParameterDeclaration;

using AliasTypeDeclarationList = std::vector<AliasTypeDeclaration *>;

using VariableDeclarationList = std::vector<VariableDeclaration *>;

using FunctionDeclarationList = std::vector<FunctionDeclaration *>;

using ExpressionList = std::vector<Expression *>;

using StatementList = std::vector<Statement *>;

using FormalParameterList = std::vector<FormalParameterDeclaration *>;

class IntegerLiteral;

class TypeDeclaration;

class IDTypeDeclaration;

class ArrayTypeDeclaration;

class AliasTypeDeclaration;

class Ident {
  SMLoc Location;
  StringRef Name;

public:
  Ident(SMLoc Loc, const StringRef &Name)
      : Location(Loc), Name(Name) {}

  SMLoc getLocation() { return Location; }

  const StringRef &getName() { return Name; }
};

using IdentList = std::vector<std::pair<SMLoc, StringRef>>;

class Declaration {
public:
  enum DeclarationKind {
    DK_Program,
    DK_Const,
    DK_Type,
    DK_Var,
    DK_Function,
    DK_Parameter
  };

private:
  const DeclarationKind Kind;

protected:
  Declaration *EnclosingDecl;
  SMLoc Location;
  StringRef Name;

public:
  Declaration(DeclarationKind Kind, Declaration *EnclosingDecl, SMLoc Loc, StringRef Name)
  : Kind(Kind), EnclosingDecl(EnclosingDecl), Location(Loc), Name(Name) {}

  DeclarationKind getKind() const { return Kind; }

  SMLoc getLocation() { return Location; }

  StringRef getName() { return Name; }

  Declaration *getEnclosingDecl() { return EnclosingDecl; }
};

class ProgramDeclaration : public Declaration {
  AliasTypeDeclarationList AliasTypeDeclarations;
  VariableDeclarationList VariableDeclarations;
  FunctionDeclarationList FunctionDeclarations;
  StatementList Statements;
public:
  ProgramDeclaration(Declaration *EnclosingDecL, SMLoc Loc,  StringRef Name)
  : Declaration(DK_Program, EnclosingDecL, Loc, Name) {}

  ProgramDeclaration(Declaration *EnclosingDecL, SMLoc Loc,  StringRef Name,
                     AliasTypeDeclarationList &TypeDeclarations, VariableDeclarationList &VariableDeclarations,
                     FunctionDeclarationList &FunctionDeclarations, StatementList &Statements)
      : Declaration(DK_Program, EnclosingDecL, Loc, Name), AliasTypeDeclarations(TypeDeclarations),
        VariableDeclarations(VariableDeclarations), FunctionDeclarations(FunctionDeclarations),
        Statements(Statements) {}

  const AliasTypeDeclarationList &getAliasTypeDeclarations() { return AliasTypeDeclarations; }

  void setAliasTypeDeclarations(AliasTypeDeclarationList &ATD) { AliasTypeDeclarations = ATD; }

  const VariableDeclarationList &getVariableDeclarations() { return VariableDeclarations; }

  void setVariableDeclarations(VariableDeclarationList &VD) { VariableDeclarations = VD; }

  const FunctionDeclarationList &getFunctionDeclarations() { return FunctionDeclarations; }

  void setFunctionDeclarations(FunctionDeclarationList &FD) { FunctionDeclarations = FD; }

  const StatementList &getStatements() { return Statements; }

  void setStatements(StatementList &L) { Statements = L; }

  static bool classof(const Declaration *D) {
    return D->getKind() == DK_Program;
  }
};

class TypeDeclaration : public Declaration {
public:
  enum TypeCategory{
    ID_Type,
    Array_Type,
    Alias_Type,
  };
private:
  const TypeCategory Category;
public:
  TypeDeclaration(Declaration *EnclosingDecL, SMLoc Loc,
                  StringRef Name, TypeCategory Category)
      : Declaration(DK_Type, EnclosingDecL, Loc, Name), Category(Category){}

  TypeCategory getCategory() const {
    return Category;
  }

  static bool classof(const Declaration *D) {
    return D->getKind() == DK_Type;
  }

  virtual TypeDeclaration *getBase() = 0;

  virtual bool isCompatible(TypeDeclaration *TD) = 0;
};

// Int or Float
class IDTypeDeclaration: public TypeDeclaration {
public:
  IDTypeDeclaration(Declaration *EnclosingDecL, SMLoc Loc, StringRef Name)
  : TypeDeclaration(EnclosingDecL, Loc, Name, ID_Type) {}

  static bool categoryOf(const TypeDeclaration *TD) {
    return TD->getCategory() == ID_Type;
  }

  TypeDeclaration * getBase() override {
    return this;
  }

  bool isCompatible(TypeDeclaration *TD) override {
    if (IDTypeDeclaration::categoryOf(TD)) {
      return dyn_cast<IDTypeDeclaration>(TD)->Name == Name;
    } else {
      return false;
    }
  }
};

class AliasTypeDeclaration: public TypeDeclaration {
  TypeDeclaration *Alias;
public:
  AliasTypeDeclaration(Declaration *EnclosingDecL, SMLoc Loc, StringRef Name,
                       TypeDeclaration *Alias)
      : TypeDeclaration(EnclosingDecL, Loc, Name, Array_Type),
        Alias(Alias){}

  TypeDeclaration *getBase() override {
    if (AliasTypeDeclaration::categoryOf(Alias)) {
      return dyn_cast<AliasTypeDeclaration>(Alias)->getBase();
    } else {
      return Alias;
    }
  }

  bool isCompatible(TypeDeclaration *TD) override {
    TypeDeclaration *Base = getBase();
    return Base->isCompatible(TD);
  }

  static bool categoryOf(const TypeDeclaration *TD) {
    return Alias_Type == TD->getCategory();
  }
};

class ArrayTypeDeclaration: public TypeDeclaration {
  IntegerLiteral *Init;
  TypeDeclaration *Type;
public:
  ArrayTypeDeclaration(Declaration *EnclosingDecL, SMLoc Loc, StringRef Name,
                       IntegerLiteral *Init, TypeDeclaration *Type)
      : TypeDeclaration(EnclosingDecL, Loc, Name, Array_Type),
      Init(Init), Type(Type){}

  IntegerLiteral *getInit() { return Init; }

  TypeDeclaration *getType(){
    return Type;
  }

  TypeDeclaration * getBase() override {
    return this;
  }

  bool isCompatible(TypeDeclaration *TD) override {
    if (IDTypeDeclaration::categoryOf(TD)) {
      return false;
    } else if (ArrayTypeDeclaration::categoryOf(TD)) {
      ArrayTypeDeclaration *pTD = dyn_cast<ArrayTypeDeclaration>(TD);
      return Type->isCompatible(pTD->getType());
    } else {
      AliasTypeDeclaration *pTD = dyn_cast<AliasTypeDeclaration>(TD);
      return pTD->isCompatible(this);
    }
  }

  static bool categoryOf(const TypeDeclaration *TD) {
    return TD->getCategory() == Array_Type;
  }
};

class VariableDeclaration : public Declaration {
  TypeDeclaration *Ty;

public:
  VariableDeclaration(Declaration *EnclosingDecL, SMLoc Loc,
                      StringRef Name, TypeDeclaration *Ty)
      : Declaration(DK_Var, EnclosingDecL, Loc, Name), Ty(Ty) {}

  TypeDeclaration *getType() { return Ty; }

  static bool classof(const Declaration *D) {
    return D->getKind() == DK_Var;
  }
};

class FormalParameterDeclaration : public Declaration {
  TypeDeclaration *Ty;
  bool IsVar;

public:
  FormalParameterDeclaration(Declaration *EnclosingDecL, SMLoc Loc,
                             StringRef Name,
                             TypeDeclaration *Ty,
                             bool IsVar)
      : Declaration(DK_Parameter, EnclosingDecL, Loc, Name), Ty(Ty),
        IsVar(IsVar) {}

  TypeDeclaration *getType() { return Ty; }

  bool isVar() const { return IsVar; }

  static bool classof(const Declaration *D) {
    return D->getKind() == DK_Parameter;
  }
};

class FunctionDeclaration : public Declaration {
  FormalParameterList Parameters;
  TypeDeclaration *RetType;
  StatementList Statements;

public:
  FunctionDeclaration(Declaration *EnclosingDecL, SMLoc Loc, StringRef Name)
      : Declaration(DK_Function, EnclosingDecL, Loc, Name) {}

  FunctionDeclaration(Declaration *EnclosingDecL, SMLoc Loc, StringRef Name,
                      FormalParameterList &Params,
                      TypeDeclaration *RetType,
                      StatementList &Statements)
      : Declaration(DK_Function, EnclosingDecL, Loc, Name),
        Parameters(Params),
        RetType(RetType),
        Statements(Statements) {}

  const FormalParameterList &getFormalParameters() {
    return Parameters;
  }

  void setFormalParameters(FormalParameterList &FP) { Parameters = FP; }

  TypeDeclaration *getRetType() { return RetType; }

  void setRetType(TypeDeclaration *Ty) { RetType = Ty; }

  const StatementList &getStatements() { return Statements; }

  void setStatements(StatementList &L) { Statements = L; }

  static bool classof(const Declaration *D) {
    return D->getKind() == DK_Function;
  }
};

class OperatorInfo {
  SMLoc Loc;
  uint32_t Kind: 16;
  uint32_t IsUnspecified: 1;

public:
  OperatorInfo()
      : Loc(), Kind(tok::unknown), IsUnspecified(true) {}

  OperatorInfo(SMLoc Loc, tok::TokenKind Kind,
               bool IsUnspecified = false)
      : Loc(Loc), Kind(Kind), IsUnspecified(IsUnspecified) {
  }

  SMLoc getLocation() const { return Loc; }

  tok::TokenKind getKind() const {
    return static_cast<tok::TokenKind>(Kind);
  }

  bool isUnspecified() const { return IsUnspecified; }
};

class Expression {
public:
  enum ExprKind {
    EK_Infix,
    EK_Prefix,
    EK_Number,
    EK_Var,
    EK_Array,
    EK_Func,
  };
private:
  const ExprKind Kind;
  TypeDeclaration *Ty;
  bool IsConstant;

protected:
  Expression(ExprKind Kind, TypeDeclaration *Ty, bool IsConst)
      : Kind(Kind), Ty(Ty), IsConstant(IsConst) {}

public:
  ExprKind getKind() const { return Kind; }

  TypeDeclaration *getType() { return Ty; }

  void setType(TypeDeclaration *T) { Ty = T; }

  bool isConst() const { return IsConstant; }
};

class InfixExpression : public Expression {
  Expression *Left;
  Expression *Right;
  const OperatorInfo Op;

public:
  InfixExpression(Expression *Left, Expression *Right, OperatorInfo Op,
                  TypeDeclaration *Ty, bool IsConst)
      : Expression(EK_Infix, Ty, IsConst), Left(Left),
        Right(Right), Op(Op) {}

  Expression *getLeft() { return Left; }

  Expression *getRight() { return Right; }

  const OperatorInfo &getOperatorInfo() { return Op; }

  static bool classof(const Expression *E) {
    return E->getKind() == EK_Infix;
  }
};

class PrefixExpression : public Expression {
  Expression *E;
  const OperatorInfo Op;

public:
  PrefixExpression(Expression *E, OperatorInfo Op,
                   TypeDeclaration *Ty, bool IsConst)
      : Expression(EK_Prefix, Ty, IsConst), E(E), Op(Op) {}

  Expression *getExpr() { return E; }

  const OperatorInfo &getOperatorInfo() { return Op; }

  static bool classof(const Expression *E) {
    return E->getKind() == EK_Prefix;
  }
};


class NumberLiteral : public Expression {
public:
  enum LiteralKind {
    NL_Int,
    NL_Float
  };
protected:
  SMLoc Loc;

  const LiteralKind LKind;
public:
  NumberLiteral(SMLoc Loc, TypeDeclaration *Ty, LiteralKind LKind)
  : Expression(EK_Number, Ty, true), Loc(Loc), LKind(LKind) {}

  LiteralKind getLiteralKind() const {
    return LKind;
  }
};

class IntegerLiteral : public NumberLiteral {
  llvm::APInt Value;
public:
  IntegerLiteral(SMLoc Loc, llvm::APInt Value,
                 TypeDeclaration *Ty)
      : NumberLiteral(Loc, Ty, NL_Int), Value(std::move(Value)) {}

  llvm::APInt &getValue() { return Value; }

  static bool classof(const Expression *E) {
    if( E->getKind() == EK_Number) {
      auto NL = dyn_cast<IntegerLiteral>(E);
      return NL->getLiteralKind() == NL_Int;
    }
    return false;
  }
};

class FloatLiteral : public NumberLiteral {
  llvm::APFloat Value;
public:
  FloatLiteral(SMLoc Loc, llvm::APFloat Value,
                 TypeDeclaration *Ty)
      : NumberLiteral(Loc, Ty, NL_Float), Value(std::move(Value)) {}

  llvm::APFloat &getValue() { return Value; }

  static bool classof(const Expression *E) {
    if( E->getKind() == EK_Number) {
      auto NL = dyn_cast<IntegerLiteral>(E);
      return NL->getLiteralKind() == NL_Float;
    }
    return false;
  }
};

class ArrayAccess : public Expression {
  Declaration *Variable;
  Expression *Subscript;
public:
  explicit ArrayAccess(VariableDeclaration *Variable, Expression *Subscript)
      : Expression(EK_Array, Variable->getType(), false),
      Variable(Variable), Subscript(Subscript) {}

  Declaration *getDecl() { return Variable; }

  Expression *getSubscript() { return Subscript; }

  static bool classof(const Expression *E) {
    return E->getKind() == EK_Array;
  }
};

class VariableAccess : public Expression {
  Declaration *Variable;
public:
  explicit VariableAccess(VariableDeclaration *Variable)
      : Expression(EK_Var, Variable->getType(), false), Variable(Variable) {}

  explicit VariableAccess(FormalParameterDeclaration *Param)
      : Expression(EK_Var, Param->getType(), false), Variable(Param) {}

  Declaration *getDecl() { return Variable; }

  static bool classof(const Expression *E) {
    return E->getKind() == EK_Var;
  }
};

//class ConstantAccess : public Expression {
//  ConstantDeclaration *Constant;
//
//public:
//  explicit ConstantAccess(ConstantDeclaration *Constant)
//      : Expression(EK_Const, Constant->getExpr()->getType(), true),
//        Constant(Constant) {}
//
//  ConstantDeclaration *geDecl() { return Constant; }
//
//  static bool classof(const Expression *E) {
//    return E->getKind() == EK_Const;
//  }
//};


class FunctionCallExpr : public Expression {
  FunctionDeclaration *Function;
  ExpressionList Parameters;

public:
  FunctionCallExpr(FunctionDeclaration *Proc,
                   ExpressionList &Parameters)
      : Expression(EK_Func, Proc->getRetType(), false),
        Function(Proc), Parameters(Parameters) {}

  FunctionDeclaration *geDeclaration() { return Function; }

  const ExpressionList &getParameters() { return Parameters; }

  static bool classof(const Expression *E) {
    return E->getKind() == EK_Func;
  }
};

class Statement {
public:
  enum StatementKind {
    SK_Assign,
    SK_FunctionCall,
    SK_If,
    SK_While,
    SK_For,
    SK_Return,
    SK_Break
  };
private:
  const StatementKind Kind;
protected:
  explicit Statement(StatementKind Kind) : Kind(Kind) {}
public:
  StatementKind getKind() const { return Kind; }
};

class AssignmentStatement : public Statement {
  Declaration *Var;
  Expression *E;

public:
  AssignmentStatement(Declaration *Var, Expression *E)
      : Statement(SK_Assign), Var(Var), E(E) {}

  Declaration *getVar() const { return Var; }
  Expression *getExpr() const { return E; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_Assign;
  }
};

class FunctionCallStatement : public Statement {
  FunctionDeclaration *Function;
  ExpressionList Parameters;

public:
  FunctionCallStatement(FunctionDeclaration *Function,
                        ExpressionList &Parameters)
      : Statement(SK_FunctionCall), Function(Function), Parameters(Parameters) {}

  FunctionDeclaration *getProc() { return Function; }

  const ExpressionList &getParams() { return Parameters; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_FunctionCall;
  }
};

class IfStatement : public Statement {
  Expression *Condition;
  StatementList IfStmts;
  StatementList ElseStmts;

public:
  IfStatement(Expression *Condition, StatementList &IfStmts,
              StatementList &ElseStmts)
      : Statement(SK_If), Condition(Condition), IfStmts(IfStmts),
        ElseStmts(ElseStmts) {}

  Expression *getCond() { return Condition; }

  const StatementList &getIfStmts() { return IfStmts; }

  const StatementList &getElseStmts() { return ElseStmts; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_If;
  }
};

class WhileStatement : public Statement {
  Expression *Condition;
  StatementList Statements;

public:
  WhileStatement(Expression *Condition, StatementList &Statements)
      : Statement(SK_While), Condition(Condition), Statements(Statements) {}

  Expression *getCondition() { return Condition; }

  const StatementList &getWhileStatements() { return Statements; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_While;
  }
};

class ForStatement : public Statement {
  Declaration *Var;
  Expression *StartExpression;
  Expression *EndExpression;
  StatementList Statements;
public:
  ForStatement(Declaration *Var, Expression *StartExpression, Expression *EndExpression)
      : Statement(SK_For), Var(Var),
      StartExpression(StartExpression), EndExpression(EndExpression){}

  const Expression *getStartExpression() { return StartExpression; }

  const Expression *getEndExpression() { return EndExpression; }

  const Declaration *getVariable() { return Var; }

  const StatementList &getForStatements() { return Statements; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_For;
  }
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement() : Statement(SK_Break){}

  static bool classof(const Statement *S) {
    return S->getKind() == SK_Break;
  }
};

class ReturnStatement : public Statement {
  Expression *ReturnValue;

public:
  explicit ReturnStatement(Expression *ReturnValue)
      : Statement(SK_Return), ReturnValue(ReturnValue) {}

  Expression *getReturnValue() { return ReturnValue; }

  static bool classof(const Statement *S) {
    return S->getKind() == SK_Return;
  }
};

#endif //LTIGER_AST_H
