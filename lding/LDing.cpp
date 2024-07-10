//
// Created by Haibo Yan on 7/10/24.
//

#include "CodeGen.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

static llvm::cl::opt<std::string>
        Input(llvm::cl::Positional,
              llvm::cl::desc("<input expression>"),
              llvm::cl::init(""));