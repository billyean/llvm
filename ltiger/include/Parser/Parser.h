//
// Created by haibo on 7/20/24.
//

#ifndef LTIGER_PARSER_H
#define LTIGER_PARSER_H
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
    if (Tok.is(ExpectedTok)) {
      advance();
      return false;
    }

    return true;
  }

  template<typename... Tokens>
  bool skipUntil(Tokens &&...Toks) {
    while (true) {
      if ((... || Tok.is(Toks)))
        return false;

      if (Tok.is(tok::eof))
        return true;
      advance();
    }
  }

  bool parseProgram(ProgramDeclaration *&D);
  bool parseDeclarationSegment(AliasTypeDeclarationList &TypeDeclarations,
                               VariableDeclarationList &VariableDeclarations,
                               FunctionDeclarationList &FunctionDeclarations);
  bool parseTypeDeclarations(AliasTypeDeclarationList &TypeDeclarations);
  bool parseVariableDeclarations(VariableDeclarationList &VariableDeclarations);
  bool parseFunctionDeclarations(FunctionDeclarationList &FunctionDeclarations);
  bool parseTypeDeclaration(TypeDeclaration *TypeDecl);

  bool parseFormalParameterList(FormalParameterList &Parameters);
  bool parseFormalParameter(FormalParameterList &Parameters);
  bool parseStatementSequence(StatementList &Statements);
  bool parseIfStatement(StatementList &Statements);
  bool parseWhileStatement(StatementList &Statements);
  bool parseForStatement(StatementList &Statements);
  bool parseReturnStatement(StatementList &Statements);
  bool parseBreakStatement(StatementList &Statements);

  bool parseExpression(Expression *&E);
  bool parseRelation(OperatorInfo &Op);
  bool parseSimpleExpression(Expression *&E);
  bool parseOrOperator(OperatorInfo &Op);
  bool parseOrLogical(Expression *&E);
  bool parseAndOperator(OperatorInfo &Op);
  bool parseAndLogical(Expression *&E);
  bool parseEqualOperator(OperatorInfo &Op);
  bool parseNEqualOperator(OperatorInfo &Op);
  bool parseLessOperator(OperatorInfo &Op);
  bool parseGreatOperator(OperatorInfo &Op);
  bool parseLessEQOperator(OperatorInfo &Op);
  bool parseGreatEQOperator(OperatorInfo &Op);
  bool parseCompare(Expression *&E);
  bool parseSubOperator(OperatorInfo &Op);
  bool parseAddOperator(OperatorInfo &Op);
  bool parseTerm(Expression *&E);
  bool parseDivOperator(OperatorInfo &Op);
  bool parseMulOperator(OperatorInfo &Op);
  bool parseFactor(Expression *&E);
  bool parsePowOperator(OperatorInfo &Op);
  bool parseExponent(Expression *&E);

  bool parseOptionalInit(NumberLiteral *&NL);

public:
  Parser(Lexer &Lex, Sema &Actions);

  ProgramDeclaration *parse();
};

#endif //LTIGER_PARSER_H
