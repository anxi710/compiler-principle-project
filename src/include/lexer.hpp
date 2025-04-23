#pragma once

#include <string>

namespace compiler::lexer {

class Token;

// abstract class lexer: define some utilities and data structure.
class Lexer {
public:
    Lexer();

    // Perfect forwarding
    template<typename T>
    Lexer(T&& text): text(std::forward<T>(text)), pos(0), peek(text[0]) {}

    virtual ~Lexer() = default;

public:
    virtual Token nextToken() = 0;

public:
    bool advance(void);

    /**
     * @brief 重置当前扫描位置
     * @param pos 设置到的位置
     */
    inline void reset(size_t pos) {
        this->pos  = pos;
        this->peek = text[pos];
        return;
    }

protected:
    std::string text; // text to be scanned
    size_t      pos;  // the next position to be scanned
    char        peek; // the next character to be scanned
};

} // namespace compiler::lexer
