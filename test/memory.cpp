#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"

using namespace llvm;

Function *createFunction(IRBuilder<> &Builder, std::string Name, Module *ModuleOb, std::vector<std::string> FunArgs) {
    // Type *u32Ty = Type::getInt32Ty(Builder.getContext());
    // Type *arrTy = ArrayType::get(u32Ty, 2);
    Type *ptrTy = Type::getInt32PtrTy(Builder.getContext());
    FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), ptrTy, false);
    Function *testFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
    return testFunc;
}

void setFuncArgs(Function *func, std::vector<std::string> FunArgs) {
    unsigned Idx = 0;
    Function::arg_iterator AI, AE;
    for (AI = func->arg_begin(), AE = func->arg_end(); AI != AE; ++AI, ++Idx)
    {
        AI->setName(FunArgs[Idx]);
    }
}

void buildTestFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
    std::vector<std::string> args;
    args.push_back("p1");
    Function *testFunc = createFunction(Builder, "test", ModuleOb, args);
    setFuncArgs(testFunc, args);

    SymbolTableList<Argument>::iterator iter = testFunc->arg_begin();
    Value *Base = &*iter;
    //Value *Base = dyn_cast<Value>Arg;
    BasicBlock *entry = BasicBlock::Create(Context, "entry", testFunc);
    Builder.SetInsertPoint(entry);
    // Read second element in ve
    Value *Offset = Builder.getInt32(1);
    Value *gep = Builder.CreateGEP(Base, Offset, "a1");
    Value *resultV = Builder.CreateLoad(gep, "result");
    Builder.CreateRet(resultV);
}

void buildSwapFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
    std::vector<std::string> args;
    args.push_back("p1");
    Function *swapFunc = createFunction(Builder, "swap", ModuleOb, args);
    setFuncArgs(swapFunc, args);

    SymbolTableList<Argument>::iterator iter = swapFunc->arg_begin();
    Value *Base = &*iter;
    //Value *Base = dyn_cast<Value>Arg;
    BasicBlock *entry = BasicBlock::Create(Context, "entry", swapFunc);
    Builder.SetInsertPoint(entry);

    Value *gep0 = Builder.CreateGEP(Base, Builder.getInt32(0), "a0");
    Value *gep1 = Builder.CreateGEP(Base, Builder.getInt32(1), "a1");
    Value *v0 = Builder.CreateLoad(gep0, "r0");
    Value *v1 = Builder.CreateLoad(gep1, "r1");
    Builder.CreateStore(v1, gep0);
    Builder.CreateStore(v0, gep1);
    Builder.CreateRet(Builder.getInt32(0));
}

int main(int argc, char *argv[]) {
    LLVMContext Context;
    // Set module name as test.
    Module *ModuleOb = new Module("memory.c", Context);
    IRBuilder<> Builder(Context);
    buildTestFunction(Context, ModuleOb, Builder);
    buildSwapFunction(Context, ModuleOb, Builder);

    //verifyFunction(*maxFunction);
    ModuleOb->dump();
    return 0;
}
