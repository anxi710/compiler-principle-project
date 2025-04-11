#pragma once

#include <string>
#include <sstream>
#include "include/token_type.hpp"

namespace compiler::lexer {

enum class TokenType;

class Token {
public:
    // 保留字 or 关键字
    static const Token END;     // end of file
    static const Token UNKNOWN; // unknown token
    static const Token IF;

public:
    Token() = delete;
    Token(TokenType type, std::string value);
    ~Token() = default;

public:
    /**
     * @brief  获取 token 的值
     * @return token value
     */
    inline const std::string getValue() {
        return this->value;
    }

    /**
     * @brief  将 token 格式化为一个 string
     * @return 格式化后的字符串
     */
    inline const std::string toString() {
        std::ostringstream oss;
        oss << "token {type: " << tokenType2str(this->type)
            << ", value: " << this->value << "}" << std::endl;
        return oss.str();
    }

private:
    TokenType   type;  // token type
    std::string value; // 组成 token 的字符串
};

} // namespace compiler::lexer
