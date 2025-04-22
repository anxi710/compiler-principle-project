#pragma once

#include <string>
#include <cassert>
#include <unordered_map>
#include "include/token.hpp"

namespace lexer::keyword {

/**
 * @brief   关键字表
 * @details 内置一个存储所有关键字的 hash map，用于查找判断指定 token 是否为关键字
 */
class KeywordTable {
public:
    KeywordTable()  = default;
    ~KeywordTable() = default;

public:
    /**
     * @brief  判断给定的输入值是否是一个关键字
     * @param  value token value]
     * @return true/false
     */
    inline bool iskeyword(std::string value) {
        return (keywords.find(value) != keywords.end());
    }

    /**
     * @brief  根据输入值获取对应 token
     * @param  value token value
     * @return keyword token
     */
    inline token::Token getKeyword(std::string value) {
        assert(keywords.find(value) != keywords.end());
        return keywords.find(value)->second;
    }

    /**
     * @brief 向关键词表中添加一个关键词
     * @param name  keyword name
     * @param token keyword token
     */
    inline void addKeyword(std::string name, token::Token token) {
        this->keywords.emplace(name, token);
    }

private:
    std::unordered_map<std::string, token::Token> keywords; // keyword hash map
};

} // namespace lexer::keyword
