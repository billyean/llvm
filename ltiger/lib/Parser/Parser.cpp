//
// Created by haibo on 7/20/24.
//

#include "Parser/Parser.h"
#include "Basic/TokenKinds.h"


namespace {
  OperatorInfo fromTok(Token Tok) {
    return OperatorInfo(Tok.getLocation(), Tok.getKind());
  }
}

Parser::Parser(Lexer &Lex, Sema &Actions)
    : Lex(Lex), Actions(Actions) {
  advance();
}

ProgramDeclaration *Parser::parse() {
  ProgramDeclaration *program = nullptr;
  parseProgram(program);
  return program;
}

bool Parser::parseProgram(ProgramDeclaration *&D) {
  auto _errorhandler = [this] { return skipUntil(); };
  if (consume(tok::kw_main))
    return _errorhandler();
  if (consume(tok::kw_let))
    return _errorhandler();
  D = Actions.actOnProgramDeclaration(Tok.getLocation(), Tok.getIdentifier());
  EnterDeclScope S(Actions, D);
  advance();
  AliasTypeDeclarationList AliasTypeDeclarations;
  VariableDeclarationList VariableDeclarations;
  FunctionDeclarationList FunctionDeclarations;
  if (parseDeclarationSegment(AliasTypeDeclarations, VariableDeclarations, FunctionDeclarations))
    return _errorhandler();

  advance();
  if (consume(tok::kw_in))
    return _errorhandler();
  advance();
  if (consume(tok::kw_begin))
    return _errorhandler();

  StatementList Statements;
  parseStatementSequence(Statements);

  Actions.actOnProgramDeclaration(D, Tok.getLocation(),
                                  Tok.getIdentifier(),
                                  AliasTypeDeclarations, VariableDeclarations, FunctionDeclarations,
                                  Statements);
  advance();
  if (consume(tok::kw_end))
    return _errorhandler();
  return false;
}

bool Parser::parseDeclarationSegment(AliasTypeDeclarationList &TypeDeclarations,
                                     VariableDeclarationList &VariableDeclarations,
                                     FunctionDeclarationList &FunctionDeclarations) {
  auto _errorhandler = [this] {
    return skipUntil(tok::identifier);
  };
  while (Tok.is(tok::kw_type)) {
    if (parseTypeDeclarations(TypeDeclarations))
      return _errorhandler();
  }
  while (Tok.is(tok::kw_var)) {
    if (parseVariableDeclarations(VariableDeclarations))
      return _errorhandler();
  }
  while (Tok.is(tok::kw_function)) {
    if (parseFunctionDeclarations(FunctionDeclarations))
      return _errorhandler();
  }
  return false;
}

bool Parser::parseTypeDeclarations(AliasTypeDeclarationList &TypeDeclarations) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semicolon);
  };

  while (Tok.is(tok::kw_type)) {
    advance();
    if (Tok.is(tok::identifier)) {
      return _errorhandler();
    }
    auto *I = new Ident(Tok.getLocation(), Tok.getIdentifier());
    advance();
    if (consume(tok::tassign)) {
      return _errorhandler();
    }
    AliasTypeDeclaration *D;
    if (parseTypeDeclaration(D))
      return _errorhandler();
    if (consume(tok::semicolon))
      return _errorhandler();
    Actions.actOnAliasTypeDeclaration(TypeDeclarations, I, D);
  }

  return false;
}


bool Parser::parseVariableDeclarations(VariableDeclarationList &VariableDeclarations) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semicolon);
  };

  while (Tok.is(tok::kw_var)) {
    advance();
    if (Tok.is(tok::identifier)) {
      return _errorhandler();
    }
    auto *I = new Ident(Tok.getLocation(), Tok.getIdentifier());
    advance();
    if (consume(tok::colon))
      return _errorhandler();
    TypeDeclaration *D;
    if (parseTypeDeclaration(D))
      return _errorhandler();

    NumberLiteral *NL;
    if (consume(tok::assign))
      parseOptionalInit(NL);
    if (consume(tok::semicolon))
      return _errorhandler();
    Actions.actOnVariableDeclaration(VariableDeclarations, I, NL, D);
  }

  return false;
}

bool Parser::parseOptionalInit(NumberLiteral *&NL) {
  if (Tok.is(tok::int_lit)) {
    NL = new IntegerLiteral(Tok.getLocation(), llvm::APInt(), Actions::IntegerType);
  }
  if (Tok.is(tok::int_lit)) {

  }

}