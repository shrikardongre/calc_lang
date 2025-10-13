#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token{
    friend class Lexer ;

    public :
    
    enum TokenKind {
        eoi,unknown,ident,number,comma,colon,plus,minus,star,slash,l_paren,r_paren,with 
    };
    
    TokenKind getkind() const {
        return Kind;
    }
    
    llvm::StringRef getText() const {
    return Text;
    }
    
    bool is(TokenKind k) const {
        return Kind==k ;
    }

    template <typename... Ts>

    bool isOneOf(TokenKind k1,TokenKind k2,Ts... ks) const {
        return is(k1) || isOneOf(k2,ks...) ;
    }

 
    private :
    TokenKind Kind ;
    llvm::StringRef Text ;
    
};

class Lexer {
    const char *BufferStart ;
    const char *BufferPtr;

    public:
    Lexer(const llvm::StringRef &Buffer){
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }

    void next (Token &token) ; // returns the next token
    private:
    void formToken(Token &Result, const char *tokEnd,Token::TokenKind kind);
    
};


#endif