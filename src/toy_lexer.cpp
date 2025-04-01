#include <cctype> // 定义有 isspace(), isalpha() 等函数
#include "include/toy_lexer.hpp"

Token ToyLexer::nextToken() {
    if (this->pos == this->text.length()) {
        return Token::END;
    }

    return Token::END;
}
