//
// Created by haibo on 7/19/24.
//

#include "Basic/Diagnostic.h"

const char *DiagnosticText[] = {
#define DIAG(ID, Level, Msg) Msg,
#include "Basic/Diagnostic.def"
};

SourceMgr::DiagKind DiagnosticKind[] = {
#define DIAG(ID, Level, Msg) SourceMgr::DK_##Level,
#include "Basic/Diagnostic.def"
};

const char *DiagnosticsEngine::getDiagnosticText(unsigned int DiagID) {
  return DiagnosticText[DiagID];
}

SourceMgr::DiagKind DiagnosticsEngine::getDiagnosticKind(unsigned int DiagID) {
  return DiagnosticKind[DiagID];
}
