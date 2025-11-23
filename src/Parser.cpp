#include "Parser.h"
#include "AST.h"
#include "Lexer.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

// The main thing to note here is that this parsecalc() is the method that denotes that whole input will be consumed as it was the first non terminal of the grammer .

// This is why here we will check if the last token is eoi.
AST* Parser::parse(){
    AST* res = parsecalc();
    expect(Token::eoi);  
    return res;
}

// calc : ("with" ident ("," ident)* ":")? expr ;
//This is the grammer we need to implement .


AST* Parser::parsecalc(){
    Expr* E;
    // Learning : SmallVector<T, N> is like a vector that pre-allocates space for N elements inside the object itself (on the stack).
    //So it does NOT use heap until the number of elements exceeds N.
    llvm::SmallVector<llvm::StringRef,8> Vars ;
    
    // The group begins with the with token.
    if(Tok.is(Token::with)){
        advance();
    
        if(expect(Token::TokenKind::ident)){
           goto _error;
        }

        Vars.push_back(Tok.getText());
        advance();

        while(Tok.is(Token::TokenKind::comma)){
            advance();
            if(expect(Token::ident)){
                goto _error;
            }
            Vars.push_back(Tok.getText());
            advance();
        }

        if (consume(Token::TokenKind::colon)){
            goto _error;
        }
    }

        E=parseExpr();
        if(Vars.empty()) return E;
        // Will look at this term later .
        else return new WithDecl(Vars,E);

// For handling the error we simply parse until the end of the line and return the nullptr for calc
        _error:
            while(!Tok.is(Token::eoi)){
                advance();
            }
            return nullptr;
        }

  // expr : term (( "+" | "-" ) term)* ;
  Expr* Parser::parseExpr(){

  Expr* left = parseTerm();

    while(Tok.isOneOf(Token::TokenKind::plus,Token::TokenKind::minus)){
        BinaryOp::Operator op ;
        if(Tok.is(Token::plus)){
            op=BinaryOp::Plus ;
        }
        else {
            op=BinaryOp::Minus;
        }
        advance();
        Expr* right = parseTerm();
        // -------------------------------------------------------------------
        // Here the left associative grammer is being followed .
        // For the expression a + b + c.
        /*
                  (+)
                 /   \
               (+)    c
              /   \
             a     b    
        */
       // --------------------------------------------------------------------
        left=new BinaryOp(op,left,right);
    }
    return left;
  }

  Expr *Parser::parseTerm() {
  Expr *Left = parseFactor();
  while (Tok.isOneOf(Token::star, Token::slash)) {
    BinaryOp::Operator Op =
        Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
    advance();
    Expr *Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseFactor() {
  Expr *Res = nullptr;
  switch (Tok.getkind()) {
  case Token::number:
    Res = new Factor(Factor::Number, Tok.getText());
    advance();
    break;
  case Token::ident:
    Res = new Factor(Factor::Ident, Tok.getText());
    advance(); 
    break;
  case Token::l_paren:
    advance();
    Res = parseExpr();
    // The consume method has the expect() in it that returns true when there is an error .
    // Here when we will parse the expression and if we do not encounter and ")" then expect will return true . So consume will also return true ,
    // But if and only if there is no error it will return false and that is only out favourable scenario hence we will stop there .
    if (!consume(Token::r_paren)){
     break;
    }
  default:
    if (!Res)
      error();
    // since the error has already been printed we need not to print it again.

    /*
    Why do we skip tokens?
    Because if we don’t, the parser would stay stuck at the bad token and loop forever or generate a bunch of cascading error messages.
    So instead, we fast-forward through garbage input until we find a safe point.
    */
    while (!Tok.isOneOf(Token::r_paren, Token::star,
                        Token::plus, Token::minus,
                        Token::slash, Token::eoi))
    { 
        advance();
    }
  }
  return Res;
}  






    
