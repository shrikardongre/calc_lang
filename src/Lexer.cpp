#include "Lexer.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Compiler.h>

namespace charinfo{
    LLVM_READNONE inline bool isWhitespace(char c){
        return c== ' ' || c=='\n' ;
    }

    LLVM_READNONE inline bool isDigit(char c){
        return c>='0' && c<='9' ;
    }

    LLVM_READNONE inline bool isLetter(char c){
       return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z');    
        }
}

void Lexer :: next (Token &token) {
    // ignore the whitespace 
    while(*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
        ++BufferPtr;
    }
    // check if there are still some characters to process 
    if(!BufferPtr){
        token.Kind=Token::eoi ;
        return ;
    }

    // check if the character is Letter 

    if (charinfo :: isLetter(*BufferPtr)){
        const char* end = BufferPtr +1 ;
        while(charinfo:: isLetter(*end)){
            ++end;
        }
        llvm::StringRef name (BufferPtr,end-BufferPtr);
        Token::TokenKind kind ;
        if(name=="with") kind =Token::with ;
        else kind ==Token::ident ;
        formToken(token,end,kind);
        return;
    }

    else if(charinfo::isDigit(*BufferPtr)){
        const char *end = BufferPtr+1;
        while(charinfo::isDigit(*end)){
            ++end;
        }
        formToken(token,end,Token::number);
        return ;
    }

    else {
        // Some fixed strings 
        switch(*BufferPtr){
            case('+'):
            formToken(token,BufferPtr+1,Token::plus);
            case('-'):
            formToken(token,BufferPtr+1,Token::minus);
            case('*'):
            formToken(token,BufferPtr+1,Token::star);
            case('/'):
            formToken(token,BufferPtr+1,Token::slash);
            case('('):
            formToken(token,BufferPtr+1,Token::l_paren);
            case(')'):
            formToken(token,BufferPtr+1,Token::r_paren);
            case(':'):
            formToken(token,BufferPtr+1,Token::colon);
            case(','):
            formToken(token,BufferPtr+1,Token::comma);

            default :
            formToken(token,BufferPtr+1, Token::unknown);
        }
        return ;
    }
}
void Lexer:: formToken (Token &token , const char* tokEnd,Token::TokenKind kind) 
{
  token.Kind=kind;
  token.Text=llvm::StringRef(BufferPtr,tokEnd-BufferPtr);
  BufferPtr=tokEnd;
}

// This next method simply returns the next character till it reaches the end.
// Then the formToken method is responsible for formation of the token by taking in account the kind 
// and the whole text that the token has where the tokEnd is coming from the const char* end or BufferPtr+1 

// This marks the end of Lexer .
    