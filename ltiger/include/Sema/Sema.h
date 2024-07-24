//
// Created by haibo on 7/23/24.
//

#ifndef LTIGER_SEMA_H
#define LTIGER_SEMA_H

#include "AST/AST.h"
#include "Basic/Diagnostic.h"
#include "Sema/Scope.h"
#include <memory>

class Sema {
  friend class EnterDeclScope;

  void enterScope(Declaration *);

  void leaveScope();

  bool isOperatorForType(tok::TokenKind Op,
                         TypeDeclaration *Ty);

  void checkFormalAndActualParameters(
      SMLoc Loc, const FormalParameterList &Formals,
      const ExpressionList &Actuals);


  static TypeDeclaration *corese(TypeDeclaration *type, TypeDeclaration *type2);

  Scope *CurrentScope;
  TypeDeclaration *CurrentTypeDeclaration;
  VariableDeclaration *CurrentVariableDeclaration;
  FunctionDeclaration *CurrentFunctionDeclaration;
  DiagnosticsEngine &Diags;

  TypeDeclaration *IntegerType;
  TypeDeclaration *BooleanType;

public:
  Sema(DiagnosticsEngine &Diags)
      : CurrentScope(nullptr),
        CurrentTypeDeclaration(nullptr),
        CurrentVariableDeclaration(nullptr),
        CurrentFunctionDeclaration(nullptr),
        Diags(Diags) {
    initialize();
  }

  void initialize();

  ProgramDeclaration *actOnProgramDeclaration(SMLoc Loc,
                                            StringRef Name);

  void actOnProgramDeclaration(ProgramDeclaration *ModDecl,
                              SMLoc Loc, StringRef Name,
                              AliasTypeDeclarationList &TypeDeclarations,
                              VariableDeclarationList &VariableDeclarations,
                              FunctionDeclarationList &FunctionDeclarations,
                              StatementList &Statements);


  void actOnAliasTypeDeclaration(AliasTypeDeclarationList &AliasTypeDeclarations,
                                 Ident *I,
                                 TypeDeclaration *ATD);

  void actOnVariableDeclaration(VariableDeclarationList &AliasTypeDeclarations,
                                Ident *I,
                                NumberLiteral *OptionalInitial,
                                TypeDeclaration *ATD);

  void actOnFunctionDeclaration(AliasTypeDeclarationList &AliasTypeDeclarations,
                                VariableDeclarationList &VariableDeclarations,
                                IdentList &Ids, Declaration *D);

  void actOnFormalParameterDeclaration(FormalParameterList &Params,
                                       IdentList &Ids, Declaration *D,
                                       bool IsVar);

  FunctionDeclaration *actOnFunctionDeclaration(SMLoc Loc, StringRef Name);

  void actOnFunctionDeclaration(
      FunctionDeclaration *FuncDecl, SMLoc Loc,
      StringRef Name, StatementList &Statements);

  void actOnAssignment(StatementList &Statements, SMLoc Loc,
                       Declaration *D,
                       Expression *E);

  void actOnFunctionCall(StatementList &Statements, SMLoc Loc,
                         Declaration *D,
                         ExpressionList &Parameters);

  void actOnIfStatement(StatementList &Statements, SMLoc Loc,
                        Expression *Condition,
                        StatementList &IfStatements,
                        StatementList &ElseStatements);

  void actOnWhileStatement(StatementList &Statements, SMLoc Loc,
                           Expression *Condition,
                           StatementList &WhileStatements);

  void actOnForStatement(StatementList &Statements, SMLoc Loc,
                         Declaration *D,
                         Expression *Start, Expression *End,
                         StatementList &ForStatements);

  void actOnBreakStatement(StatementList &Statements, SMLoc Loc);

  void actOnReturnStatement(StatementList &Statements, SMLoc Loc,
                            Expression *RetVal);

  Expression *actOnExpression(Expression *Left, Expression *Right,
                              const OperatorInfo &Op);

  Expression *actOnSimpleExpression(Expression *Left, Expression *Right,
                                    const OperatorInfo &Op);

  Expression *actOnTerm(Expression *Left, Expression *Right,
                        const OperatorInfo &Op);

  Expression *actOnPrefixExpression(Expression *E,
                              const OperatorInfo &Op);

  Expression *actOnIntegerLiteral(SMLoc Loc, StringRef Literal);

  Expression *actOnFloatLiteral(SMLoc Loc, StringRef Literal);

  Expression *actOnVariable(VariableDeclaration *D);

//  Expression *actOnFunctionCall(Decl *D, ExprList &Params);
//
//  Decl *actOnQualIdentPart(Decl *Prev, SMLoc Loc,
//                           StringRef Name);
};

class EnterDeclScope {
  Sema &Semantics;

public:
  EnterDeclScope(Sema &Semantics, Declaration *D)
      : Semantics(Semantics) {
    Semantics.enterScope(D);
  }

  ~EnterDeclScope() { Semantics.leaveScope(); }
};

#endif //LTIGER_SEMA_H
