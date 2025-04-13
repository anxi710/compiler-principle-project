#pragma once

#include <string>
#include <vector>
#include <regex>

enum class TokenType {
    IDENT, NUMBER, PLUS, MINUS, STAR, SLASH,
    LPAREN, RPAREN, END, INVALID
};

struct Token {
    TokenType type;
    std::string text;
};

class Lexer {
public:
    Lexer(const std::string& src);
    Token next();

private:
    std::string source;
    size_t pos;
};
