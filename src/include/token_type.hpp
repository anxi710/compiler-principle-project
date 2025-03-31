#ifndef TOKEN_TYPE_HPP
#define TOKEN_TYPE_HPP

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

#endif
