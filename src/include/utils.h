#ifndef UTILS_H
#define UTILS_H

/* 本文件给出一系列实用函数的声明 */

#include "token.h"

/**
 * @brief  向 fd 指定的设备格式化打印/写入一个 Token
 * @param  fd    文件描述符
 * @param  token 词法单元
 * @return void
 */
void printToken(int fd, struct Token token);

#endif
