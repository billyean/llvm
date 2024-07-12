//
// Created by haibo on 7/10/24.
//

#ifndef LDU_DIAGNOSTIC_H
#define LDU_DIAGNOSTIC_H
#include "Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <utility>

using namespace llvm;

namespace diag {
  enum {
  #define DIAG(ID, Level, Msg) ID,
  #include "Basic/Diagnostic.def"
  };
}

class DiagnosticsEngine {
  SourceMgr &SrcMgr;
  unsigned NumErrors;

public:
  DiagnosticsEngine(SourceMgr &SrcMgr): SrcMgr(SrcMgr), NumErrors(0) {}

  unsigned numErros() {
    return NumErrors;
  }

  template <typename ... Args> void report (SMLoc Loc, unsigned DiagID, Args &&... Arguments) {
    std::string Msg = llvm::formatv(getDiagnosticText(DiagID),
                                    std::forward<Args>(Arguments)...).str();
    SourceMgr::DiagKind Kind = getDiagnosticKind(DiagID);
    SrcMgr.PrintMessage(Loc, Kind, Msg);
    NumErrors += (Kind == SourceMgr::DK_Error);
  }

  static const char *getDiagnosticText(unsigned DiagID);

  static SourceMgr::DiagKind getDiagnosticKind(unsigned DiagID);
};



#endif //LDU_DIAGNOSTIC_H
