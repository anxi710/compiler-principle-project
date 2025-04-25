#pragma once

#include <string>
#include <optional>
#include "token.hpp"

namespace lexer::base {

// abstract class lexer: define some utilities and data structure.
class Lexer {
public:
    Lexer() : text(""), pos(0), peek('\0') {}

    // Perfect forwarding
    template<typename T>
    Lexer(T&& text): text(std::forward<T>(text)), pos(0), peek(text[0]) {}

    virtual ~Lexer() = default;

public:
    virtual std::optional<token::Token> nextToken() = 0;

public:
    /**
     * @brief 重置当前扫描位置
     * @param pos 设置到的位置
     */
    inline void reset(size_t pos) {
        this->pos  = pos;
        this->peek = text[pos];
    }

protected:
    std::string text; // text to be scanned
    std::size_t pos;  // the next position to be scanned
    char        peek; // the next character to be scanned
};

} // namespace lexer::base
