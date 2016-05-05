enum TOKEN_TYPE {
  EOF_TOKEN = 0,
  NUMBERIC_TOKEN,
  IDENTIFIER_TOKEN,
  PAREN_TOKEN,
  DEF_TOKEN
};

static int Numeric_Val;
static std::string Identifier_string;

static int get_token() {
  static int LastChar = ' ';

  while(isspace(LastChar))
    lastChar = fgetc(file);

  if (isalpha(LastChar)) {
    Identifier_string = LastChar;
    while (isalnum((LastChar = fgetc(file))))
      Identifier_string += LastChar;

    // keyword def, this should be a bug that if we have identifier like def1
    // will be considered as def.
    if(Identifier_string == "def")
      return DEF_TOKEN;
    return IDENTIFIER_TOKEN;
  }

  if (isdigit(LastChar)) {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = fget(file);
    } while (isdigit(LastChar));

    Numeric_Val = strtod(NumStr.c_str(), 0);
    return NUMERIC_TOKEN;
  }

  if (LastChar == '#') {
    do LastChar = fget(file);
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar == EOF)  return EOF_TOKEN;
    int ThisChar = LastChar;
    LastChar = fget(file);
    return ThisChar;
  }
}

class BaseAST {
public:
  virtual ~BaseAST();
}

class VariableAST : public BaseAST {
  std::string Var_Name;

public:
  VariableAST(std::string &name) : Var_Name(name) {}
}

class NumericAST : public BaseAST {
  int Number_Val;

public:
  NumericAST(int val) : Number_Val(val) {}
}

class BinariesAST : public BaseAST {
  std::string Bin_Operator;

  BaseAST *LHS, *RHS;

public:
  BinariesAST(std::string op, BaseAST *lhs, BaseAST *rhs) : Bin_Operator(op), LHS(lhs), RHS(rhs) {}
}

class FuncDeclAST : public BaseAST {
  std::string Func_Name;
  std::vector<std::string> Arguments;

public:
  FuncDeclAST(const std::string &name, const std::vector<std::string> &args) : Func_Name(name), Arguments(args) {}
}


class FuncDefAST : public BaseAST {
  FuncDeclAST *Func_Decl;
  BaseAST *Body;

public:
  FuncDefAST(FuncDeclAST *proto, BaseAST *body) : Func_Decl(proto), Body(body) {}
}

class FuncCallAST : public BaseAST {
  std::string Function_Callee;
  std::vector<BaseAST*> Function_Arguments;

public:
  FuncCallAST(const std::string &callee, std::vector<BaseAST*> &args):
      Function_Callee(callee), Function_Arguments(args) {}
}
