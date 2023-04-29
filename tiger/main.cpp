#include <iostream>

#include "antlr4-runtime.h"
#include "TigerLexer.h"
#include "TigerParser.h"

using namespace antlrcpptest;
using namespace antlr4;

int main(int , const char **) {
  ANTLRInputStream input(u8"🍴 = 🍐 + \"😎\";(((x * π))) * µ + ∰; a + (x * (y ? 0 : 1) + z);");
  TigerLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  TigerParser parser(&tokens);
  tree::ParseTree* tree = parser.tiger_program();

  std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

  return 0;
}
