#pragma once

#include <string>

#include "token_type.hpp"
#include "util/position.hpp"

namespace lexer::token
{

enum class Type : std::uint8_t;

class Token
{
   public:
    Token() = default;

    explicit Token(const Type& t, std::string v, const util::Position& p = util::Position{0, 0})
        : type(t), value(std::move(v)), pos(p)
    {
    }

    Token(const Token& other) = default;

    Token(Token&& other) noexcept : type(other.type), value(std::move(other.value)), pos(other.pos)
    {
    }

    ~Token() = default;

    auto operator=(const Token& rhs) -> Token& = default;

    auto operator==(const Token& rhs) -> bool
    {
        // == 并不考虑位置！
        return this->type == rhs.type && this->value == rhs.value;
    }

   public:
    /**
     * @brief  获取 token 的值
     * @return token value
     */
    [[nodiscard]] auto getValue() const -> const std::string& { return this->value; }

    /**
     * @brief  获取 token 的类型
     * @return token type
     */
    [[nodiscard]] auto getType() const -> Type { return this->type; }

    /**
     * @brief  获取 token 的位置
     * @return Position
     */
    [[nodiscard]] auto getPos() const -> util::Position { return this->pos; }

    /**
     * @brief 设置 token 所在的文本位置
     * @param p struct Position {row, col}
     */
    void setPos(const util::Position& p) { this->pos = p; }

    /**
     * @brief 设置 token 所在的文本位置
     * @param r row
     * @param c col
     */
    void setPos(std::size_t r, std::size_t c)
    {
        this->pos.row = r;
        this->pos.col = c;
    }

    [[nodiscard]] auto toString() const -> std::string;

   private:
    Type type;           // token type
    std::string value;   // 组成 token 的字符串
    util::Position pos;  // position
};

}  // namespace lexer::token
