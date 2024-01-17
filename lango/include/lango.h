#ifndef Lango_h
#define Lango_h

#include <string>
#include <memory>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"


using namespace llvm;

class Lango {
public:
	Lango() : _ctx(std::make_unique<LLVMContext>()),
        _builder(std::make_unique<IRBuilder<>>(*_ctx)),
        _module(std::make_unique<Module>("Lango", *_ctx)){

        createSquare();
        createMain();
    }

    void exec(const std::string& source) {
    	_module->print(llvm::outs(), nullptr);
    	saveToModule(source);
    }
private:
    void saveToModule(const std::string& llvm_file) {
        std::error_code error_code;
        llvm::raw_fd_stream outLL(llvm_file, error_code);
        _module->print(outLL, nullptr);
    }

    void createMain() {
        std::vector<Type*> Voids;
        FunctionType *FT =
            FunctionType::get(Type::getInt32Ty(*_ctx), Voids, false);
        Function *F =
            Function::Create(FT, Function::ExternalLinkage, "main", _module.get());
        BasicBlock *BB = BasicBlock::Create(*_ctx, "entry", F);
        _builder->SetInsertPoint(BB);


        llvm::FunctionType *printfType = FunctionType::get(_builder->getInt32Ty(), _builder->getInt8Ty()->getPointerTo(), true);
        llvm::FunctionCallee printfFunc = _module->getOrInsertFunction("printf", printfType);


        Constant *formatStr = ConstantDataArray::getString(*_ctx, "square(%d) = %d\n");
        GlobalVariable *formatStrVar = new GlobalVariable(*_module, formatStr->getType(), true, GlobalValue::PrivateLinkage, formatStr, ".str");
        Constant *zero = ConstantInt::get(Type::getInt32Ty(*_ctx), 0);
        std::vector<llvm::Constant*> indices = {zero, zero};
        Constant *strPtr = ConstantExpr::getGetElementPtr(formatStr->getType(), formatStrVar, indices);


        Constant *thirteen = ConstantInt::get(Type::getInt32Ty(*_ctx), 13);
        std::vector<Value *> squarefArgs;
        squarefArgs.push_back(thirteen);


        FunctionType *squareType = FunctionType::get(_builder->getInt32Ty(), _builder->getInt32Ty(), true);
        FunctionCallee squareFunc = _module->getOrInsertFunction("square", squareType);
        Value* squareValue = _builder->CreateCall(squareFunc, squarefArgs);


        std::vector<Value *> printfArgs {strPtr, thirteen, squareValue};

        Value* printfValue = _builder->CreateCall(printfFunc, printfArgs);


        auto constant0 = ConstantInt::get(*_ctx, APInt(32, 0));
        _builder->CreateRet(constant0);
    }

    void createSquare() {
        std::vector<Type*> Ints { Type::getInt32Ty(*_ctx)};
        FunctionType *FT =
            FunctionType::get(Type::getInt32Ty(*_ctx), Ints, false);
        Function *F =
            Function::Create(FT, Function::ExternalLinkage, "square", _module.get());
        BasicBlock *BB = BasicBlock::Create(*_ctx, "entry", F);
        _builder->SetInsertPoint(BB);

        Function::arg_iterator args = F->arg_begin();
        Value *x = args++;
        x->setName("x");

        Value *result = _builder->CreateMul(x, x, "result");
        _builder->CreateRet(result);
    }

	std::unique_ptr<LLVMContext> _ctx;

    std::unique_ptr<IRBuilder<>> _builder;

	std::unique_ptr<Module> _module;
};

#endif