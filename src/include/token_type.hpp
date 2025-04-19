#pragma once

#include <string>

namespace compiler::lexer {

enum class TokenType {
    // Group 0
    END, // end of file
    UNKNOWN, // unknown value

    // Group 1
    // lookahead = 1 (LA(1))
    IF, ELSE,
    WHILE,
    ID,
    I32,
    LET,
    RETURN,
    MUT,
    FN,
    FOR,
    IN,
    LOOP,
    BREAK,
    CONTINUE,
    INT,

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
    // lookahead = 2
    
    ASSIGN,     //  =
    OP_MINUS,   //  -
    OP_MUL,     //  *
    OP_DIV,     //  /
    OP_GT,      //  >
    OP_LT,      //  <
    DOT,        //  .

    // Group 3
    // arbitrary LA
    OP_EQ,      //  ==
    OP_NEQ      //  !=
    OP_GE,      //  >=
    OP_LE,      //  <=
    DOTS,       //  ..
    ARROW,      //  ->
    SIN_COM,    //  //
    LMUL_COM,   //  /*
    RMUL_COM   //  */    
};

const std::string tokenType2str(TokenType type);

} // namespace compiler::lexer
