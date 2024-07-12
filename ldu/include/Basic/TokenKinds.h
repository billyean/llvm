//
// Created by haibo on 7/11/24.
//

#ifndef LDU_TOKENKINDS_H
#define LDU_TOKENKINDS_H
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
#endif //LDU_TOKENKINDS_H
