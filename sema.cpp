#include "sema.h"
#include "AST.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
    class DeclCheck: public ASTVisitor{
        llvm::StringSet<> Scope;
        bool HasError;
        enum ErrorType{Twice,Not};

        void error(ErrorType ET, llvm::StringRef V){
            llvm::errs()<<"The variable "<<V<<" is "<< (ET==Twice ? "already" : "not")<<"declared\n" ;
            HasError=true;
        }

        public:
        DeclCheck() : HasError(false){}
        bool hasError() {return HasError;}    

        /*
          The names must be stored in a set here Scope so now is the time of parsing the node and checking if it is already in the set .
          Why check only factor ? Since The factor holds the name or number that the variable has.
        */

        virtual void visit(Factor &Node) override {
          if(Node.getKind()==Factor::Ident){
            if(Scope.find(Node.getVal()) == Scope.end()){
                error(Not,Node.getVal());
            }
          }
        }

        virtual void visit(WithDecl &Node) override {
            for(auto I = Node.begin(),E=Node.end();I !=E;++I){
                if(!Scope.insert(*I).second){
                    error(Twice,*I);
                }
                if(Node.getExpr()){
                    Node.getExpr()->accept((*this));
                }
                else {
                    HasError=true;
                }
            }
        } 

        // For a binaryOp node we should just check if both the sides are presend and visited.
         
        virtual void visit(BinaryOp &Node) override {
            if(Node.getLeft()){
                Node.getLeft()->accept(*this);
            }
            else{
                HasError=true;
            }
            if(Node.getright()){
                Node.getright()->accept(*this);
            }
            else{
                HasError=true;
            }
        }


    }; 
}

bool sema::semantic(AST *tree){
    if(!tree) return false ;
    DeclCheck check;
    tree->accept(check);
    return check.hasError();
}