#include "include/toy_lexer.hpp"

Token ToyLexer::nextToken() {
    if (this->pos == this->text.length()) {
        return Token(TokenType::END);
    }
}
