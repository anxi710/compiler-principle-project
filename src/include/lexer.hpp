#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include "token.hpp"

// abstract class lexer: define some utilities and data structure.
class Lexer {
public:
    Lexer() : text(""), pos(0), peek('\0') {}
    Lexer(const std::string text) : text(text), pos(0), peek(text[0]) {}
    ~Lexer() = default;

    /**
     * @brief 虚函数，获取下一个词法单元
     */
    virtual Token nextToken() = 0;

    /**
     * @brief  向前扫描一个字符
     * @return 是否到达文本结尾
     */
    bool advance() {
        this->pos++;
        if (this->pos == this->text.length()) {
            return true;
        } else {
            this->peek = this->text[this->pos];
            return false;
        }
    }

    /**
     * @brief 重置当前扫描位置
     * @param pos 设置到的位置
     */
    void reset(size_t pos) {
        this->pos  = pos;
        this->peek = text[pos];
        return;
    }

protected:
    std::string text; // text to be scanned
    size_t      pos;  // the next position to be scanned
    char        peek; // the next character to be scanned
};

#endif
