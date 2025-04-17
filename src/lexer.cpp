#include "include/lexer.hpp"

namespace compiler::lexer {

/* constructor */

Lexer::Lexer() : text(""), pos(0), peek('\0') {
}

Lexer::Lexer(const std::string text) : text(text), pos(0), peek(text[0]) {
}

/* constructor */

/* member function definition */

/**
 * @brief  向前扫描一个字符
 * @return 是否到达文本结尾
 */
bool Lexer::advance(void) {
    this->pos++;
    if (this->pos == this->text.length()) {
        return true;
    } else {
        this->peek = this->text[this->pos];
        return false;
    }
}

/* member function definition */

} // namespace compiler::lexer
