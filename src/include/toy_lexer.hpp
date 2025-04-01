#ifndef TOY_LEXER_HPP
#define TOY_LEXER_HPP

#include <string>
#include <memory>
#include "keyword_table.hpp"
#include "lexer.hpp"

class ToyLexer : public Lexer {
public:
    ToyLexer() {
        last_matched_pos          = 0;
        longest_valid_prefix_pos  = 0;
        longest_valid_prefix_type = TokenType::UNKNOWN;
    }
    ToyLexer(const std::string text) : Lexer(text) {
        ToyLexer();
    }
    ~ToyLexer() = default;

public:
    /**
     * @brief 设置待分析的文本
     * @param text 文本字符串
     */
    inline void setText(std::string text) {
        this->text = text;
    }

    /**
     * @brief 设置关键字表
     * @param p_keyword_table 关键字表的独占指针
     */
    inline void setKeywordTable(std::unique_ptr<KeywordTable> p_keyword_table) {
        this->p_keyword_table = std::move(p_keyword_table);
    }
    /**
     * @brief  获取下一个词法单元
     * @return next token
     */
    Token nextToken();

private:
    size_t       last_matched_pos;          // 上一次匹配成功的位置
    size_t       longest_valid_prefix_pos;  // 当前匹配到的最长合法前缀位置
    TokenType    longest_valid_prefix_type; // 当前匹配到的最长合法前缀对应的 token 类型

    std::unique_ptr<KeywordTable> p_keyword_table;
};

#endif
