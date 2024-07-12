//
// Created by haibo on 7/11/24.
//

#include "Basic/Diagnostic.h"
#include "Basic/Ver.h"
#include "Parser/Parser.h"
#include "CodeGen/CodeGen.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/TargetParser/Host.h"

using namespace llvm;

static codegen::RegisterCodeGenFlags CGF;

static llvm::cl::opt<std::string> InputFile(llvm::cl::Positional, llvm::cl::desc("<input-files>"), cl::init("-"));
static llvm::cl::opt<std::string> OutputFilename("o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"));
static llvm::cl::opt<std::string> MTriple("mtriple", llvm::cl::desc("Override target tripe for module"));
static llvm::cl::opt<bool> EmitLLVM(
    "emit-llvm",
    llvm::cl::desc("Emit IR"),
    llvm::cl::init(false));
static const char *Head = "ldu - Module2 compiler";

void printVersion(llvm::raw_ostream &OS) {
  OS << Head << " " << getVersion() << "\n";
  OS << "  Default target: " << llvm::sys::getDefaultTargetTriple() << "\n";
  std::string CPU(llvm::sys::getHostCPUName());
  OS << "  Host CPU: " << CPU << "\n\n";
  OS.flush();
  llvm::TargetRegistry::printRegisteredTargetsForVersion(OS);
  exit(EXIT_SUCCESS);
}

llvm::TargetMachine *createTargetMachine(const char *Argv0) {
  llvm::Triple Triple = llvm::Triple(
      !MTriple.empty() ? llvm::Triple::normalize(MTriple) : llvm::sys::getDefaultTargetTriple());
  llvm::TargetOptions TargetOptions =
      codegen::InitTargetOptionsFromCodeGenFlags(Triple);
  std::string CPUStr = codegen::getCPUStr();
  std::string FeatureStr = codegen::getFeaturesStr();

  std::string Error;
  const llvm::Target *Target = llvm::TargetRegistry::lookupTarget(codegen::getMArch(), Triple, Error);
  if (!Target) {
    llvm::WithColor::error(llvm::errs(), Argv0) << Error;
    return nullptr;
  }

  return Target->createTargetMachine(Triple.getTriple(), CPUStr, FeatureStr,
                                                        TargetOptions,
                                                        codegen::getRelocModel());
}

bool emit(StringRef Argv0, llvm::Module *M, llvm::TargetMachine *TM, StringRef InputFileName) {
  auto FileType = codegen::getFileType();
  if (OutputFilename.empty()) {
    if (InputFileName == "-") {
      OutputFilename = "-";
    } else {
      if (InputFileName.ends_with("mod")) {
        OutputFilename = InputFileName.drop_back(4).str();
      } else {
        OutputFilename = InputFileName.str();
      }
      switch (FileType) {
        case llvm::CGFT_AssemblyFile:
          OutputFilename.append(EmitLLVM ? ".ll" : ".s");
          break;
        case llvm::CGFT_ObjectFile:
          OutputFilename.append(".o");
          break;
        case llvm::CGFT_Null:
          OutputFilename.append(".null");
          break;
      }
    }
  }

  std::error_code EC;
  sys::fs::OpenFlags OpenFlags = sys::fs::OF_None;

  if (FileType == CGFT_AssemblyFile) {
    OpenFlags |= sys::fs::OF_TextWithCRLF;
  }
  auto Out = std::make_unique<llvm::ToolOutputFile>(OutputFilename, EC, OpenFlags);
  if(EC) {
    WithColor::error(llvm::errs(), Argv0) << EC.message() << "\n";
    return false;
  }

  legacy::PassManager PM;
  if (FileType == CGFT_AssemblyFile && EmitLLVM) {
    PM.add(createPrintModulePass(Out->os()));
  } else {
    if (TM->addPassesToEmitFile(PM, Out->os(), nullptr, FileType)) {
      WithColor::error(llvm::errs(), Argv0)  << "No support for file type\n";
      return false;
    }
  }
  PM.run(*M);
  Out->keep();
  return true;
}

int main(int argc_, const char **argv_) {
  llvm::InitLLVM X(argc_, argv_);
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmPrinters();
  InitializeAllAsmParsers();
  llvm::cl::SetVersionPrinter(&printVersion);
  llvm::cl::ParseCommandLineOptions(argc_, argv_, Head);

  if (codegen::getMCPU() == "help" ||
      std::any_of(codegen::getMAttrs().begin(),
                  codegen::getMAttrs().end(),
                  [](const std::string &a) {
                    return a == "help";
                  })) {
    auto Triple = llvm::Triple(LLVM_DEFAULT_TARGET_TRIPLE);
    std::string ErrMsg;
    if (auto Target = llvm::TargetRegistry::lookupTarget(
        Triple.getTriple(), ErrMsg)) {
      llvm::errs() << "Targeting " << Target->getName()
                   << ". ";
      // This prints the available CPUs and features of the
      // target to stderr...
      Target->createMCSubtargetInfo(
          Triple.getTriple(), codegen::getCPUStr(),
          codegen::getFeaturesStr());
    } else {
      llvm::errs() << ErrMsg << "\n";
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }

  llvm::outs() << "Inputfile : " << InputFile << "\n";

  llvm::TargetMachine *TM = createTargetMachine(argv_[0]);
  if (!TM)
    exit(EXIT_FAILURE);
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
      FileOrErr = llvm::MemoryBuffer::getFile(InputFile);
  if (std::error_code BufferError = FileOrErr.getError()) {
    llvm::WithColor::error(llvm::errs(), argv_[0]) << "Error reading " << InputFile << ": " << BufferError.message() << "\n";
  }
  llvm::SourceMgr SrcMgr;
  DiagnosticsEngine Diags(SrcMgr);

  SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr),
                            llvm::SMLoc());
  auto TheLexer = Lexer(SrcMgr, Diags);
  auto TheSema = Sema(Diags);
  auto TheParser = Parser(TheLexer, TheSema);
  auto *ModuleDecl = TheParser.parse();

  if (ModuleDecl && !Diags.numErros()) {
    llvm::LLVMContext Ctx;
    if (CodeGen *CG =
        CodeGen::create(Ctx, TM)) {
        llvm::outs() << "InputFile : " << InputFile << "\n";
      std::unique_ptr<llvm::Module> M =  CG->run(ModuleDecl, InputFile);
      if (!emit(argv_[0], M.get(), TM, InputFile)) {
        llvm::WithColor::error(llvm::errs(), argv_[0]) << "Error writing output\n";
      }
      delete CG;
    }

//  llvm::SmallVector<const char *, 256> argv(argv_ + 1,
//                                            argv_ + argc_);
//
//  llvm::outs() << "LDU : "
//               << getVersion() << "\n";

//  for (const char *F: argv_) {
//    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
//        FileOrErr = llvm::MemoryBuffer::getFile(F);
//    if (std::error_code BufferError =
//        FileOrErr.getError()) {
//      llvm::errs() << "Error reading " << F << ": "
//                   << BufferError.message() << "\n";
//      continue;
//    }
//
//    llvm::SourceMgr SrcMgr;
//    DiagnosticsEngine Diags(SrcMgr);
//
//    // Tell SrcMgr about this buffer, which is what the
//    // parser will pick up.
//    SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr),
//                              llvm::SMLoc());
//
//    auto TheLexer = Lexer(SrcMgr, Diags);
//    auto TheSema = Sema(Diags);
//    auto TheParser = Parser(TheLexer, TheSema);
//    auto ModuleDecl = TheParser.parse();
//    CodeGen CodeGenerator = CodeGen::create()

  }
}