#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include <string>
#include <vector>

using namespace llvm;

typedef SmallVector<BasicBlock *, 16> BBList;
typedef SmallVector<Value *, 16> ValList;

Function *createFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb, std::vector<std::string> FunArgs) {
    std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Builder.getContext()));
    FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
    Function *maxFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
    return maxFunc;
}

void setFuncArgs(Function *func, std::vector<std::string> FunArgs) {
    unsigned Idx = 0;
    Function::arg_iterator AI, AE;
    for (AI = func->arg_begin(), AE = func->arg_end(); AI != AE; ++AI, ++Idx)
    {
        AI->setName(FunArgs[Idx]);
    }
}

void buildMaxFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
  // function max's two parameters
  std::vector<std::string> args;
  args.push_back("p1");
  args.push_back("p2");
  Function *maxFunction = createFunc(Builder, "max", ModuleOb, args);
  setFuncArgs(maxFunction, args);
  BasicBlock *label = BasicBlock::Create(Context, "label", maxFunction);
  Builder.SetInsertPoint(label);
  SymbolTableList<Argument>::iterator iter = maxFunction->arg_begin();
  Argument *Arg1 = &*iter;
  iter++;
  Argument *Arg2 = &*iter;
  Value *Compare = Builder.CreateICmpULT(Arg1, Arg2);
  Value *BV = Builder.CreateZExt(Compare, Type::getInt32Ty(Context), "booltmp");
  Value *Condtn = Builder.CreateICmpNE(BV, Builder.getInt32(0), "ifcond");
  BasicBlock *ThenBB = BasicBlock::Create(Context, "then", maxFunction);
  BasicBlock *ElseBB = BasicBlock::Create(Context, "else", maxFunction);
  BasicBlock *MergeBB = BasicBlock::Create(Context, "ifcont", maxFunction);
  Builder.CreateCondBr(Condtn, ThenBB, ElseBB);
  Builder.SetInsertPoint(ThenBB);
  Builder.CreateRet(Arg1);

  Builder.SetInsertPoint(ElseBB);
  Builder.CreateRet(Arg2);
}

void buildSumFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
  std::vector<std::string> args;
  args.push_back("p1");
  Function *sumFunction = createFunc(Builder, "sum", ModuleOb, args);
  setFuncArgs(sumFunction, args);

  BasicBlock *entry = BasicBlock::Create(Context, "", sumFunction);
  Builder.SetInsertPoint(entry);

  SymbolTableList<Argument>::iterator iter = sumFunction->arg_begin();
  Argument *Arg1 = &*iter;

  AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(Context), 0, "sums");
  Builder.CreateStore(Builder.getInt32(0), Alloca);

  BasicBlock *LoopBB = BasicBlock::Create(Context, "loop", sumFunction);
  BasicBlock *AfterBB = BasicBlock::Create(Context, "afterloop", sumFunction);

  Builder.CreateBr(LoopBB);
  Builder.SetInsertPoint(LoopBB);
  Value *StartVal = Builder.getInt32(1);
  PHINode *IndVar = Builder.CreatePHI(Type::getInt32Ty(Context), 2, "i");
  IndVar->addIncoming(StartVal, entry);

  // Loop body
  Value *sumV = Builder.CreateLoad(Alloca, "suml");
  Value *Add = Builder.CreateAdd(IndVar, sumV);
  Builder.CreateStore(Add, Alloca);

  Value *StepVal = Builder.getInt32(1);
  Value *NextVal = Builder.CreateAdd(IndVar, StepVal, "nextval");
  Value *EndCond = Builder.CreateICmpULT(IndVar, Arg1);
  Value *BV = Builder.CreateZExt(EndCond, Type::getInt32Ty(Context), "booltmp");
  EndCond = Builder.CreateICmpNE(BV, Builder.getInt32(0), "loopcond");
  BasicBlock *LoopEndBB = Builder.GetInsertBlock();
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  Builder.SetInsertPoint(AfterBB);
  IndVar->addIncoming(NextVal, LoopEndBB);
  sumV = Builder.CreateLoad(Alloca, "suml");
  Builder.CreateRet(sumV);
}

int main(int argc, char *argv[]) {
    LLVMContext Context;
    // Set module name as test.
    Module *ModuleOb = new Module("test.c", Context);
    IRBuilder<> Builder(Context);
    buildMaxFunction(Context, ModuleOb, Builder);
    buildSumFunction(Context, ModuleOb, Builder);

    ModuleOb->getOrInsertGlobal("glv", Builder.getInt32Ty());
    GlobalVariable *glv = ModuleOb->getNamedGlobal("glv");
    glv->setLinkage(GlobalValue::CommonLinkage);
    glv->setAlignment(4);
    glv->setInitializer(Builder.getInt32(13));

    //verifyFunction(*maxFunction);
    ModuleOb->dump();
    return 0;
}
