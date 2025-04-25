#pragma once

#include "lexer.hpp"
#include "keyword_table.hpp"

namespace lexer::impl {

class ToyLexer : public base::Lexer {
public:
    ToyLexer() : Lexer("") {
        initKeywordTable();
    }

    template<typename T>
    ToyLexer(T&& text) : Lexer(std::forward<T>(text)) {
        initKeywordTable();
    }

    ~ToyLexer() = default;

public: // virtual function
    std::optional<token::Token> nextToken();

public:
    /**
     * @brief 重新设置待分析的文本
     * @param text 文本字符串
     */
    inline void resetText(std::string&& text) {
        this->text = std::move(text);
    }

private:
    void initKeywordTable();

private:
    keyword::KeywordTable keyword_table; // 关键词表
};

} // namespace lexer::impl
