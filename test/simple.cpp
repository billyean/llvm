#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

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

//===--------------------------------------------------------------------===//
/// Create a function max
/// int test(int p1, int p2)
/// {
///     if (p1 > p2) {
///         return p1
///     } else {
///         return p2;
///     }
/// }
void buildMaxFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
  // function max's two parameters
  std::vector<std::string> args;
  args.push_back("p1");
  args.push_back("p2");
  Function *maxFunction = createFunc(Builder, "max", ModuleOb, args);
  setFuncArgs(maxFunction, args);
  BasicBlock *entry = BasicBlock::Create(Context, "", maxFunction);
  Builder.SetInsertPoint(entry);


  //SymbolTableList<Argument>::iterator iter = maxFunction->arg_begin();
  Function::arg_iterator iter = maxFunction->arg_begin();
  Argument *Arg1 = &*iter;
  iter++;
  Argument *Arg2 = &*iter;
  Value *Compare = Builder.CreateICmpULT(Arg2, Arg1);
  // Value *BV = Builder.CreateZExt(Compare, Type::getInt32Ty(Context), "booltmp");
  // Value *Condtn = Builder.CreateICmpNE(BV, Builder.getInt32(0), "ifcond");
  // BasicBlock *ThenBB = BasicBlock::Create(Context, "then", maxFunction);
  // BasicBlock *ElseBB = BasicBlock::Create(Context, "else", maxFunction);
  // BasicBlock *MergeBB = BasicBlock::Create(Context, "ifcont", maxFunction);
  // Builder.CreateCondBr(Condtn, ThenBB, ElseBB);
  // Builder.SetInsertPoint(ThenBB);
  // Builder.CreateRet(Arg1);
  //
  // Builder.SetInsertPoint(ElseBB);
  // Above will generate IR as follows:
  // define i32 @sum(i32 %p1) {
  //   %sums = alloca i32
  //   store i32 0, i32* %sums
  //   br label %loop
  // loop:                                             ; preds = %loop, %0
  //   %i = phi i32 [ 1, %0 ], [ %nextval, %loop ]
  //   %suml = load i32, i32* %sums
  //   %1 = add i32 %i, %suml
  //   store i32 %1, i32* %sums
  //   %nextval = add i32 %i, 1
  //   %2 = icmp ult i32 %i, %p1
  //   %booltmp = zext i1 %2 to i32
  //   %loopcond = icmp ne i32 %booltmp, 0
  //   br i1 %loopcond, label %loop, label %afterloop
  // afterloop:                                        ; preds = %loop
  //   %suml1 = load i32, i32* %sums
  //   ret i32 %suml1
  // }

  // By using opt -O3 -S simple.ll we can get simpler instruction as below:
  // define i32 @max(i32 %p1, i32 %p2) #0 {
  // label:
  //   %0 = icmp ult i32 %p1, %p2
  //   %p1.p2 = select i1 %0, i32 %p1, i32 %p2
  //   ret i32 %p1.p2
  // }
  // Replace first implementation as follows:
  // Here we need give InsertAtEnd entry, which is extremely important to insert
  // this select in current position, this does looks like a bug for LLVM.
  // we should always expect to insert in current position even we don't give
  // current position.
  // Submit a bug to LLVM database
  // https://llvm.org/bugs/show_bug.cgi?id=27578
  SelectInst *Select  = SelectInst::Create(Compare,
        Arg1, Arg2, "p1.p2", entry);
  Builder.CreateRet(Select);
}

//===--------------------------------------------------------------------===//
/// Create a function max
/// int test(int p1)
/// {
///     int sum = 0;
///     int i;
///     for (i = 0; i < p1; i++) {
///         sum += i;
///     }
///     return sum;
/// }
void buildSumFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
  std::vector<std::string> args;
  args.push_back("p1");
  Function *sumFunction = createFunc(Builder, "sum", ModuleOb, args);
  setFuncArgs(sumFunction, args);

  BasicBlock *entry = BasicBlock::Create(Context, "", sumFunction);
  Builder.SetInsertPoint(entry);

  Function::arg_iterator iter = sumFunction->arg_begin();
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
    glv->setAlignment(MaybeAlign(4));
    glv->setInitializer(Builder.getInt32(13));

    //verifyFunction(*maxFunction);
    //ModuleOb->dump();
    ModuleOb->print(errs(), nullptr);
    return 0;
}
