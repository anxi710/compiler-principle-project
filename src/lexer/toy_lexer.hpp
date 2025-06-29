#pragma once

#include "keyword_table.hpp"
#include "lexer.hpp"

namespace lexer::impl
{

class ToyLexer : public base::Lexer
{
   public:
    ToyLexer() = delete;

    template <typename T>
    explicit ToyLexer(T&& t) : Lexer(std::forward<T>(t))
    {  // Perfect forwarding
        initKeywordTable();
    }

    ~ToyLexer() override = default;

   public:  // virtual function
    auto nextToken() -> std::expected<token::Token, error::LexError> override;

   private:
    void initKeywordTable();

   private:
    keyword::KeywordTable keyword_table;  // 关键词表
};

}  // namespace lexer::impl
