#pragma once

#include <string>
#include <memory>
#include "include/lexer.hpp"

namespace compiler::lexer {

class KeywordTable;

class ToyLexer : public Lexer {
public:
    ToyLexer();
    ToyLexer(const std::string text);
    ~ToyLexer() = default;

public: // virtual function
    Token nextToken();

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
    inline void setKeywordTable(std::unique_ptr<KeywordTable>& p_keyword_table) {
        this->p_keyword_table = std::move(p_keyword_table);
    }

private:
    size_t       last_matched_pos;          // 上一次匹配成功的位置
    size_t       longest_valid_prefix_pos;  // 当前匹配到的最长合法前缀位置
    TokenType    longest_valid_prefix_type; // 当前匹配到的最长合法前缀对应的 token 类型

    std::unique_ptr<KeywordTable> p_keyword_table;
};

} // namespace compiler::lexer
