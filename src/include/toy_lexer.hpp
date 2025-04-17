#pragma once

#include <string>
#include "include/lexer.hpp"
#include "include/keyword_table.hpp"

namespace compiler::lexer {

class ToyLexer : public Lexer {
public:
    ToyLexer();
    ToyLexer(const std::string text);
    ~ToyLexer() = default;

public: // virtual function
    Token nextToken(void);

public:
    /**
     * @brief 设置待分析的文本
     * @param text 文本字符串
     */
    inline void setText(std::string text) {
        this->text = text;
    }

private:
    void initKeywordTable(void);

private:
    size_t       last_matched_pos;          // 上一次匹配成功的位置
    size_t       longest_valid_prefix_pos;  // 当前匹配到的最长合法前缀位置
    TokenType    longest_valid_prefix_type; // 当前匹配到的最长合法前缀对应的 token 类型
    KeywordTable keyword_table;             // 关键词表
};

} // namespace compiler::lexer
