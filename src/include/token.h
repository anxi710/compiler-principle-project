#ifndef TOKEN_H
#define TOKEN_H

/* 定义词法单元 token */

/*---------- Token Type ----------*/
enum TokenType { END, INT }; //TODO 补充其余的 token 类型

/*---------- Token Type ----------*/

/**
 * @brief
 */
struct Token {
    enum TokenType type;
    const char*    value;
};

#endif
