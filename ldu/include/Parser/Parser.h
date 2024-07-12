//
// Created by haibo on 7/11/24.
//

#ifndef LDU_PARSER_H
#define LDU_PARSER_H


#include "Lexer/Lexer.h"
#include "llvm/Support/raw_ostream.h"
#include "AST/AST.h"
#include "Sema/Sema.h"

class Parser {
  Lexer &Lex;

  Token Tok;

  Sema &Actions;

  DiagnosticsEngine &getDiagnostics() const {
    return Lex.getDiagnostics();
  }

  void advance() {
    Lex.next(Tok);
  }

  bool expect(tok::TokenKind ExpectedTok) {
    if (Tok.is(ExpectedTok)) {
      return false;
    }
    // There must be a better way!
    const char *Expected =
        tok::getPunctuatorSpelling(ExpectedTok);
    if (!Expected)
      Expected = tok::getKeywordSpelling(ExpectedTok);
    llvm::StringRef Actual(Tok.getLocation().getPointer(),
                           Tok.getLength());
    getDiagnostics().report(Tok.getLocation(),
                            diag::err_expected, Expected,
                            Actual);
    return true;
  }

  bool consume(tok::TokenKind ExpectedTok) {
    if (expect(ExpectedTok)) {
      advance();
      return false;
    }

    return true;
  }

  template <typename... Tokens>
  bool skipUntil(Tokens &&...Toks) {
    while (true) {
      if ((... || Tok.is(Toks)))
        return false;

      if (Tok.is(tok::eof))
        return true;
      advance();
    }
  }

  bool parseCompilationUnit(ModuleDeclaration *&D);
  bool parseImport();
  bool parseBlock(DeclList &Decls, StmtList &Stmts);
  bool parseDeclaration(DeclList &Decls);
  bool parseConstantDeclaration(DeclList &Decls);
  bool parseVariableDeclaration(DeclList &Decls);
  bool parseProcedureDeclaration(DeclList &ParentDecls);
  bool parseFormalParameters(FormalParamList &Params,
                             Decl *&RetType);
  bool parseFormalParameterList(FormalParamList &Params);
  bool parseFormalParameter(FormalParamList &Params);
  bool parseStatementSequence(StmtList &Stmts);
  bool parseStatement(StmtList &Stmts);
  bool parseIfStatement(StmtList &Stmts);
  bool parseWhileStatement(StmtList &Stmts);
  bool parseReturnStatement(StmtList &Stmts);
  bool parseExpList(ExprList &Exprs);
  bool parseExpression(Expr *&E);
  bool parseRelation(OperatorInfo &Op);
  bool parseSimpleExpression(Expr *&E);
  bool parseAddOperator(OperatorInfo &Op);
  bool parseTerm(Expr *&E);
  bool parseMulOperator(OperatorInfo &Op);
  bool parseFactor(Expr *&E);
  bool parseQualident(Decl *&D);
  bool parseIdentList(IdentList &Ids);
public:
  Parser(Lexer &Lex , Sema &Actions);

  ModuleDeclaration *parse();
};

#endif //LDU_PARSER_H
