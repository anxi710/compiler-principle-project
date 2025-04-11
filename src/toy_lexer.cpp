#include <cctype> // 定义有 isspace(), isalpha() 等函数
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/token_type.hpp"
#include "include/keyword_table.hpp"

namespace compiler::lexer {

/* constructor` */

ToyLexer::ToyLexer() {
    last_matched_pos          = 0;
    longest_valid_prefix_pos  = 0;
    longest_valid_prefix_type = TokenType::UNKNOWN;
}

ToyLexer::ToyLexer(const std::string text) : Lexer(text) {
    ToyLexer();
}

/* constructor` */

/* member function definition */

/**
 * @brief  获取下一个词法单元
 * @return next token
 */
Token ToyLexer::nextToken() {
    if (this->pos == this->text.length()) {
        return Token::END;
    }

    return Token::END;
}

/* member function definition */

} // namespace compiler::lexer
