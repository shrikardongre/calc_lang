#include "codegen.h"
#include "AST.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalValue.h>

using namespace llvm;

// Each compilation unit is represented in LLVM by the Module class and the
// visitor has a pointer to the module called M.

// The Builder design pattern is used for easy IR generation.
namespace {
class ToIRVisitor : public ASTVisitor {
  Module *M;
  IRBuilder<> Builder;
  Type *VoidTy;
  Type *Int32Ty;
  PointerType *PtrTy;
  Constant *Int32zero;
  Value *V;
  StringMap<Value *> nameMap;

public:
  ToIRVisitor(Module *M) : M(M), Builder(M->getContext()) {

    VoidTy = Type::getVoidTy(M->getContext());
    Int32Ty = Type::getInt32Ty(M->getContext());
    PtrTy = PointerType::getUnqual(M->getContext());
    Int32zero = ConstantInt::get(Int32Ty, 0, true);
  }

  // For each function a function type must be created .
  void run(AST *Tree) {
    // First we will start with defining the main() in LLVM IR that will server
    // as the entry point.
    FunctionType *MainFTy = FunctionType::get(Int32Ty, {Int32Ty, PtrTy}, false);
    Function *MainFn =
        Function::Create(MainFTy, GlobalValue::ExternalLinkage, "main", M);
    // This is the way of creating a basicblock in LLVM we label it entry and
    // the basic block the the main function that we just created .
    BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
    Builder.SetInsertPoint(BB);

    // Now we can traverse the tree
    Tree->accept(*this);

    FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);
    Function *CalcWriteFn = Function::Create(
        CalcWriteFnTy, GlobalValue::ExternalLinkage, "calc_write", M);
    Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});
    Builder.CreateRet(Int32zero);
  }

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      V = nameMap[Node.getVal()];
    } else {
      int intval;
      Node.getVal().getAsInteger(10, intval);
      V = ConstantInt::get(Int32Ty, intval, true);
    }
  }

  virtual void visit(BinaryOp &Node) override {
    Node.getLeft()->accept(*this);
    Value *Left = V;
    Node.getright()->accept(*this);
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
  };

  virtual void visit(WithDecl &Node) override {
    FunctionType *ReadFty = FunctionType::get(Int32Ty, {PtrTy}, false);
    Function *ReadFn =
        Function::Create(ReadFty, GlobalValue::ExternalLinkage, "calc_read", M);
    for (auto I = Node.begin(), E = Node.end(); I != E; ++I) {
      StringRef Var = *I;

      // Create call to calc_read function.
      Constant *StrText = ConstantDataArray::getString(M->getContext(), Var);
      GlobalVariable *Str =
          new GlobalVariable(*M, StrText->getType(),
                             /*isConstant=*/true, GlobalValue::PrivateLinkage,
                             StrText, Twine(Var).concat(".str"));
      CallInst *Call = Builder.CreateCall(ReadFty, ReadFn, {Str});

      nameMap[Var] = Call;
    }

    Node.getExpr()->accept(*this);
  }
};

} // namespace

void Codegen::compile(AST *Tree) {
  LLVMContext Ctx;
  Module *M = new Module("calc.expr", Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);
  M->print(outs(), nullptr);
}
