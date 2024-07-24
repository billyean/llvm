//
// Created by haibo on 7/18/24.
//

#ifndef LTIGER_TOKENKINDS_H
#define LTIGER_TOKENKINDS_H
namespace tok {
  enum TokenKind : unsigned int {
#define TOK(ID) ID,
#include "TokenKinds.def"
    NUM_TOKENS
  };

  const char *getTokenName(TokenKind Kind);
  const char *getPunctuatorSpelling(TokenKind Kind);
  const char *getKeywordSpelling(TokenKind Kind);
}
#endif //LTIGER_TOKENKINDS_H
