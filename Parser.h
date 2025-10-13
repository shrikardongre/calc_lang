#ifndef PARSER_H
#define PARSER_H
// This AST.h will be taken care of later .
#include "AST.h" 
#include "Lexer.h"
// This header is used as a replacement of <iostream> abiding the guidelines of LLVM coding standards.
#include "llvm/Support/raw_ostream.h"

class Parser {
    // This Lex will do the work of getting the next token from the input .
    Lexer &Lex;
    // this Tok will store the next token and will be given to the next() method .
    Token Tok;
    // incase any error is detected .
    bool Error;

    // defining some utility functions.
    void error(){
        llvm::errs()<< "Unexpected Token :" << Tok.getText() <<"\n" ;
        Error=true;
    }

    void advance(){Lex.next((Tok));}
    bool expect(Token::TokenKind Kind){
        if(Tok.getkind()!=Kind){
            error();
            return true ;
        }
        return false ;
    }

    // If the Token kind is the expected one then we will simmply advance().
    
    bool consume(Token::TokenKind kind){
        if(expect(kind)){
            return true;
        }
        advance();
        return false ;
    }

    // Now till now we have gathered  a token now based upon its kind we need to provide a method defining how to parse it .

    AST* parsecalc();
    Expr* parseExpr();
    Expr* parseTerm();
    Expr* parseFactor();

    public :
        Parser(Lexer &Lex) : Lex(Lex),Error(false){
            advance() ;
        }

        bool hasError() {
            return Error;
        }

        // This will be the main entry point for parsing .

        AST* parse();

};


#endif