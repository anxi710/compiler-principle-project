#pragma once

#include <string>

namespace lexer::token {

// token 类型
enum class Type {
    DEFAULT, // 默认值，无意义

    // Group 0
    END, // end of file

    // Group 1
    ID, INT, // identifier, integer
    IF, ELSE,
    WHILE, FOR,
    I32,
    LET,
    RETURN,
    MUT,
    FN,
    IN,
    LOOP,
    BREAK, CONTINUE,

    Ref,        //  &
    LPAREN,     //  (
    RPAREN,     //  )
    LBRACE,     //  {
    RBRACE,     //  }
    LBRACK,     //  [
    RBRACK,     //  ]
    SEMICOLON,  //  ;
    COLON,      //  :
    COMMA,      //  ,

    OP_PLUS,    //  +

    // Group 2
    ASSIGN,     //  =
    OP_MINUS,   //  -
    OP_MUL,     //  *
    OP_DIV,     //  /
    OP_GT,      //  >
    OP_LT,      //  <
    DOT,        //  .

    OP_EQ,      //  ==
    OP_NEQ,     //  !=
    OP_GE,      //  >=
    OP_LE,      //  <=
    DOTS,       //  ..
    ARROW,      //  ->
    SIN_COM,    //  //
    LMUL_COM,   //  /*
    RMUL_COM    //  */
};

const std::string tokenType2str(Type type);

} // namespace lexer::token
