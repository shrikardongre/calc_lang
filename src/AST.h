// The result of parsing an input program is an AST .

#ifndef _AST_H 
#define _AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"


// We forward declare these classes so that the ASTvisitor knows each class to visit.

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;


// Declaring the virtual functions so that we can evaluate the nodes in AST
class ASTVisitor{
    public:
    virtual void visit(AST &){};
    virtual void visit(Expr &){};
    virtual void visit(Factor &) = 0;
    virtual void visit(BinaryOp &) = 0;
    virtual void visit(WithDecl &) = 0;
};

class AST{
    public:
    virtual ~AST(){}
    virtual void  accept(ASTVisitor &V)=0;
};

class Expr : public AST {
    public:
    Expr() {}
};

// This class will store the number or the name of the variable 
class Factor : public Expr {
public:
  enum ValueKind { Ident, Number };

private:
  ValueKind Kind;
  llvm::StringRef Val;

public:
  Factor(ValueKind Kind, llvm::StringRef Val): Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

// This WithDecl class stores the declared variables and the expression.
class WithDecl : public AST {
  llvm::SmallVector<llvm::StringRef, 8> Vars;
  Expr *E;

public:
  WithDecl(llvm::SmallVector<llvm::StringRef, 8> Vars,Expr *E) : Vars(Vars), E(E) {}
  llvm::SmallVector<llvm::StringRef, 8>::const_iterator begin() { return Vars.begin(); }

  llvm::SmallVector<llvm::StringRef, 8>::const_iterator end() { return Vars.end(); }
  Expr *getExpr() { return E; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

// This BinaryOp class holds the data needed for evaluating and expression

class BinaryOp:public Expr {
    public:
    enum Operator {Plus,Minus,Mul,Div} ;
    BinaryOp(Operator Op,Expr* left,Expr* right){
        this->left=left;
        this->right=right;
        this->op=Op;
    }
    Expr* getLeft() {return left;}
    Expr* getright() {return right;}
    Operator getOperator() {return op;}

    virtual void accept(ASTVisitor &V){
        V.visit(*this);
    }
    
    private:
    Expr* left;
    Expr* right;
    Operator op;
};


#endif