#ifndef TOY_LEXER_HPP
#define TOY_LEXER_HPP

#include "keyword_table.hpp"
#include "lexer.hpp"

class ToyLexer : public Lexer {
public:
    ToyLexer() = delete;
    ToyLexer(const std::string text) : Lexer(text) {}
    ~ToyLexer() = default;

    /**
     * @brief  获取下一个词法单元
     * @return next token
     */
    Token nextToken();
private:
    size_t       last_matched_pos;          // 上一次匹配成功的位置
    size_t       longest_valid_prefix_pos;  // 当前匹配到的最长合法前缀位置
    TokenType    longest_valid_prefix_type; // 当前匹配到的最长合法前缀对应的 token 类型
    KeywordTable keyword_table;
};

#endif
