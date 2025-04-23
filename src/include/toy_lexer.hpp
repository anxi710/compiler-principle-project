#pragma once

#include "include/lexer.hpp"
#include "include/keyword_table.hpp"

namespace compiler::lexer {

class ToyLexer : public Lexer {
public:
    ToyLexer();

    template<typename T>
    ToyLexer(T&& text) : Lexer(std::forward<T>(text)) {
        initKeywordTable();
    }

    ~ToyLexer() = default;

public: // virtual function
    Token nextToken(void);

public:
    /**
     * @brief 设置待分析的文本
     * @param text 文本字符串
     */
    inline void setText(std::string&& text) {
        this->text = std::move(text);
    }

private:
    void initKeywordTable(void);

private:
    KeywordTable keyword_table;             // 关键词表
};

} // namespace compiler::lexer
