//
// Created by Haibo Yan on 7/10/24.
//

#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class ToIRVisitor : public ASTVisitor {
        Module *M;
        IRBuilder<> Builder;
        Type *VoidTy;
        Type *Int32Ty;
        PointerType *PtrTy;
        Constant *Int32Zero;
        Value *V;
        StringMap<Value *> nameMap;
    public:
        ToIRVisitor(Module *M): M(M), Builder(M->getContext()) {
            VoidTy = Type::getVoidTy(M->getContext());
            Int32Ty = Type::getInt32Ty(M->getContext());
            PtrTy = PointerType::getUnqual(M->getContext());
            Int32Zero = ConstantInt::get(Int32Ty, 0L, true);
        }

        void run(AST *Tree) {
            FunctionType *MainTy = FunctionType::get(
                    Int32Ty, {Int32Ty, PtrTy}, false);
            Function *MainFn = Function::Create(
                    MainTy, GlobalValue::ExternalLinkage, "main", M);
            BasicBlock *BB = BasicBlock::Create(M->getContext(),
                                                "entry",
                                                MainFn);
            Builder.SetInsertPoint(BB);
            Tree->accept(*this);

            FunctionType *CalcWriteFnTy = FunctionType::get(
                    VoidTy, {Int32Ty}, false);
            Function *CalcWriteFn = Function::Create(
                    CalcWriteFnTy, GlobalValue::ExternalLinkage, "calc_write", M);
            Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});
            Builder.CreateRet(Int32Zero);
        }

        void visit(WithDecl &Node) override {
            FunctionType *ReadFnTy = FunctionType::get(
                    Int32Ty, {PtrTy}, false);
            Function *ReadFn = Function::Create(
                    ReadFnTy, GlobalValue::ExternalLinkage, "calc_read", M);
            for (auto I = Node.begin(); I != Node.end(); ++I) {
                StringRef Var = *I;
                Constant *StrText = ConstantDataArray::getString(M->getContext(), Var);
                GlobalVariable *Str = new GlobalVariable(
                        *M, StrText->getType(),
                        true, GlobalValue::PrivateLinkage,
                        StrText, Twine(Var).concat(".str"));
                CallInst *Call = Builder.CreateCall(ReadFnTy, ReadFn, {Str});
                nameMap[Var] = Call;
            }
            Node.getExpr()->accept(*this);
        }

        void visit(Factor &Node) override {
            if (Node.getKind() == Factor::Ident) {
                V = nameMap[Node.getVal()];
            } else {
                int intval;
                Node.getVal().getAsInteger(10, intval);
                V = ConstantInt::get(Int32Ty, intval, true);
            }
        }

        void visit(BinaryOp &Node) override {
            Node.getLeft()->accept(*this);
            Value *Left = V;
            Node.getRight()->accept(*this);
            Value *Right = V;

            switch (Node.getOperator()) {
                case BinaryOp::Plus:
                    V = Builder.CreateNSWAdd(Left, Right);
                    break;
                case BinaryOp::Minus:
                    V = Builder.CreateNSWSub(Left, Right);
                    break;
                case BinaryOp::Mul:
                    V = Builder.CreateNSWMul(Left, Right);
                    break;
                case BinaryOp::Div:
                    V = Builder.CreateSDiv(Left, Right);
                    break;
            }
        }
    };
}

void CodeGen::compile(AST *Tree) {
    LLVMContext Ctx;
    Module *M = new Module("calc.expr", Ctx);
    ToIRVisitor ToIR(M);
    ToIR.run(Tree);
    M->print(outs(), nullptr);
}

