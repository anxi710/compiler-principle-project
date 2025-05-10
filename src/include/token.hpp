#pragma once

#include <string>
#include "token_type.hpp"
#include "lexer_position.hpp"

namespace lexer::token {

enum class Type;

class Token {
public:
    Token() = default;

    explicit Token(const Type& t, const std::string& v, const base::Position& p = base::Position{0, 0})
        : type(t), value(v), pos(p) {}

    Token(const Token& other) : type(other.type), value(other.value), pos(other.pos) {}

    Token(Token&& other) : type(std::move(other.type)),
        value(std::move(other.value)), pos(std::move(other.pos)) {}

    ~Token() = default;

    Token& operator=(const Token& rhs) = default;

    bool operator==(const Token& rhs) {
        // == 并不考虑位置！
        return this->type == rhs.type && this->value == rhs.value;
    }

public:
    /**
     * @brief  获取 token 的值
     * @return token value
     */
    inline const std::string& getValue() const {
        return this->value;
    }

    /**
     * @brief  获取 token 的类型
     * @return token type
     */
    inline const Type getType() const {
        return this->type;
    }

    /**
     * @brief  获取 token 的位置
     * @return Position
     */
    inline const base::Position getPos() const {
        return this->pos;
    }

    /**
     * @brief 设置 token 所在的文本位置
     * @param p struct Position {row, col}
     */
    inline void setPos(const base::Position& p) {
        this->pos = p;
    }

    /**
     * @brief 设置 token 所在的文本位置
     * @param r row
     * @param c col
     */
    inline void setPos(std::size_t r, std::size_t c) {
        this->pos.row = r;
        this->pos.col = c;
    }

    const std::string toString() const;

private:
    Type           type;  // token type
    std::string    value; // 组成 token 的字符串
    base::Position pos;   // position
};

} // namespace lexer::token
