#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/* 定义符号表 */

#include "token.h"

struct Symbol {
    const TokenType type;
    const char*     value;
};

struct SymbolTable {
    struct Symbol* symbols;
    int            size;
    int            capacity;
};

#endif
