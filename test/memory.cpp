#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;


Function *createFunction(IRBuilder<> &Builder, std::string Name,
  Module *ModuleOb, Type *resultType, Type *paramType) {
    FunctionType *funcType = FunctionType::get(resultType, paramType, false);
    return Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
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
/// Create a function call tests like
/// int test(int* p1)
/// {
///     return *(p1 + 1);
/// }
void buildTestFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
    std::vector<std::string> args;
    args.push_back("p1");
    Function *testFunc = createFunction(Builder, "test", ModuleOb,
                Builder.getInt32Ty(), Type::getInt32PtrTy(Context));
    setFuncArgs(testFunc, args);

//    SymbolTableList<Argument>::iterator iter = testFunc->arg_begin();
    llvm::Function::arg_iterator iter = testFunc->arg_begin();
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

//===--------------------------------------------------------------------===//
/// Create a function swap 1st element and 2nd element in a array
/// void swap(int* p1)
/// {
///     int t = p1[0];
///     p1[0] = p1[1];
///     p1[1] = t;
/// }
void buildSwapFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
    std::vector<std::string> args;
    args.push_back("p1");
    Function *swapFunc = createFunction(Builder, "swap", ModuleOb,
                Builder.getVoidTy(), Type::getInt32PtrTy(Context));
    setFuncArgs(swapFunc, args);

    Function::arg_iterator iter = swapFunc->arg_begin();
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
}

//===--------------------------------------------------------------------===//
/// Create a function insert that insert elements to a vector
/// void insert(vector<int> v)
/// {
///     std::vector<int>::iterator it = v.begin();
///     for (int i = 0; i < 4; i++) {
///         v.insert(it, (i + 1) * 7);
///     }
/// }
void buildInsertFunction(LLVMContext &Context, Module *ModuleOb, IRBuilder<> Builder) {
    std::vector<std::string> args;
    args.push_back("v");
    Function *insertFunc = createFunction(Builder, "insert", ModuleOb,
            Builder.getVoidTy(), VectorType::get(Type::getInt32Ty(Context), 4));
    setFuncArgs(insertFunc, args);

    Function::arg_iterator iter = insertFunc->arg_begin();
    Value *Base = &*iter;
    BasicBlock *entry = BasicBlock::Create(Context, "entry", insertFunc);
    Builder.SetInsertPoint(entry);

    for (unsigned int i = 0; i < 4; i++) {
        Builder.CreateInsertElement(Base,
                Builder.getInt32((i + 1) * 7),  /* Insert value */
                Builder.getInt32(i)); /* Insert position */
    }
}

int main(int argc, char *argv[]) {
    LLVMContext Context;
    // Set module name as test.
    Module *ModuleOb = new Module("memory.c", Context);
    IRBuilder<> Builder(Context);
    buildTestFunction(Context, ModuleOb, Builder);
    buildSwapFunction(Context, ModuleOb, Builder);
    buildInsertFunction(Context, ModuleOb, Builder);

    //verifyFunction(*maxFunction);
    ModuleOb->print(dbgs(), nullptr);
//    ModuleOb->dump();
    return 0;
}
