//
// Created by haibo on 7/15/24.
//

#ifndef LTIGER_LLVM_H
#define LTIGER_LLVM_H
#include "llvm/Support/Casting.h"

namespace llvm {
  class SMLoc;
  class SourceMgr;
  template <typename T, typename A> class StringMap;
  class StringRef;
  class raw_ostream;
} // namespace llvm

using llvm::cast;
using llvm::cast_or_null;
using llvm::dyn_cast;
using llvm::dyn_cast_or_null;
using llvm::isa;

using llvm::raw_ostream;
using llvm::SMLoc;
using llvm::SourceMgr;
using llvm::StringMap;
using llvm::StringRef;
#endif //LTIGER_LLVM_H
