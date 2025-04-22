#pragma once

#include <string>
#include "include/token_type.hpp"

namespace lexer::token {

enum class Type;

class Token {
public:
    // 关键字
    static const Token END; // end of file
    static const Token IF;
    static const Token FN;
    static const Token IN;
    static const Token I32;
    static const Token LET;
    static const Token FOR;
    static const Token MUT;
    static const Token ELSE;
    static const Token LOOP;
    static const Token BREAK;
    static const Token WHILE;
    static const Token RETURN;
    static const Token CONTINUE;

public:
    Token() : type(Type::DEFAULT), value("") {}
    Token(Type type, std::string value);
    ~Token() = default;

    Token& operator=(const Token& rhs) = default;

    bool operator==(const Token& rhs) {
        return this->type == rhs.type && this->value == rhs.value;
    }

public:
    /**
     * @brief  获取 token 的值
     * @return token value
     */
    inline const std::string& getValue() {
        return this->value;
    }

    const std::string toString();

private:
    Type        type;  // token type
    std::string value; // 组成 token 的字符串
};

} // namespace lexer::token
