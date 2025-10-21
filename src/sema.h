#ifndef SEMA_H
#define SEMA_H

#include "AST.h"
#include "Lexer.h"

class sema {
public:
  bool semantic(AST *Tree);
};

#endif