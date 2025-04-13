#pragma once
#include "lexer.hpp"

class Parser {
public:
    Parser(Lexer& lexer);
    void parse_expr(); // 可以扩展为返回 AST 节点
private:
    Token current;
    Lexer& lexer;

    void advance();
    void parse_term();
    void parse_factor();
    void match(TokenType expected);
};
