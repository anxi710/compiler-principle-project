#ifndef KEYWORD_TABLE_HPP
#define KEYWORD_TABLE_HPP

#include <string>
#include <unordered_map>
#include "token.hpp"

/**
 * @brief   关键字表
 * @details 内置一个存储所有关键字的 hash map，用于查找判断指定 token 是否为关键字
 */
class KeywordTable {
public:
    KeywordTable() {
        this->keywords["if"] = Token::IF;
    }

    ~KeywordTable() = default;

    bool isKeyword(std::string value) {
        return (keywords.find(value) != keywords.end());
    }

private:
    std::unordered_map<std::string, Token> keywords;
};

#endif
