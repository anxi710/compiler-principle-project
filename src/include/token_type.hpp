#ifndef TOKEN_TYPE_HPP
#define TOKEN_TYPE_HPP

#include <iostream>
#include <string>

enum class TokenType {
    // Group 0
    END, // end of file
    UNKNOWN, // unknown value

    // Group 1
    // lookahead = 1 (LA(1))
    DOT, ADD, SUB,
    IF, ELSE,
    ID,
    INT,
    WS,

    // Group 2
    // lookahead = 2

    // Group 3
    // arbitrary LA

};

/**
 * @brief  TokenType 转 string
 * @param  type enum class TokenType 中的一个
 * @return 对应的 string
 */
const std::string tokenType2str(TokenType type);

#endif
