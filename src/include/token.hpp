#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <sstream>
#include <string>
#include "token_type.hpp"

class Token {
public:
    static const Token END;
    static const Token IF;

public:
    Token(TokenType type, std::string value) : type(type), value(value) {}

    const std::string getValue() {
        return this->value;
    }

    const std::string toString() {
        std::ostringstream oss;
        oss << "token {type: ";
    }

private:
    TokenType   type;
    std::string value; // 组成 token 的字符串
};

const Token Token::END {TokenType::END, ""};
const Token Token::IF  {TokenType::IF, "if"};

#endif
