#pragma once

#include <string>
#include "include/token_type.hpp"

namespace compiler::lexer {

enum class TokenType;

class Token {
public:
    // 关键字
    static const Token END;     // end of file
    static const Token I32;
    static const Token LET;
    static const Token IF;
    static const Token ELSE;
    static const Token WHILE;
    static const Token RETURN;
    static const Token MUT;
    static const Token FN;
    static const Token FOR;
    static const Token IN;
    static const Token LOOP;
    static const Token BREAK;
    static const Token CONTINUE;

public:
    Token() : type(TokenType::UNKNOWN), value("") {}
    Token(TokenType type, std::string value);
    ~Token() = default;

    Token& operator=(const Token& rhs) = default;

public:
    /**
     * @brief  获取 token 的值
     * @return token value
     */
    inline const std::string getValue(void) {
        return this->value;
    }

    const std::string toString(void);

private:
    TokenType   type;      // token type
    std::string value;     // 组成 token 的字符串
};

} // namespace compiler::lexer
