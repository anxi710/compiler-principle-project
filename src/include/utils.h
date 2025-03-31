#ifndef UTILS_H
#define UTILS_H

/* 本文件给出一系列实用函数的声明 */
#include <stdio.h>
#include <stdbool.h>

/*----------- Token ----------*/
struct Token;

/**
 * @brief  向指定文件句柄格式化打印/写入一个 Token
 * @param  out   输出文件句柄
 * @param  token 词法单元
 * @return void
 */
void printToken(FILE* out, struct Token token);

/*----------- Token ----------*/

/*---------- Lexer ----------*/

// 在 <ctype.h> 中定义了 int isspace(int c) 函数，因此删除了 isWhiteSpace() 函数

/*---------- Lexer ----------*/

/*---------- Parser ----------*/

/*---------- Parser ----------*/

#endif
