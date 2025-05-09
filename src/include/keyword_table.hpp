#pragma once

#include <string>
#include <cassert>
#include <unordered_map>
#include "token_type.hpp"

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
     * @param  v token value
     * @return true / false
     */
    inline bool iskeyword(std::string v) const {
        return (keywords.find(v) != keywords.end());
    }

    /**
     * @brief  根据输入值获取对应 token type
     * @param  v token value
     * @return keyword token type
     */
    inline token::Type getKeyword(std::string v) const {
        assert(keywords.find(v) != keywords.end());
        return keywords.find(v)->second;
    }

    /**
     * @brief 向关键词表中添加一个关键词类型
     * @param n keyword name
     * @param t keyword token type
     */
    inline void addKeyword(std::string n, token::Type t) {
        this->keywords.emplace(n, t);
    }

private:
    std::unordered_map<std::string, token::Type> keywords; // keyword hash map
};

} // namespace lexer::keyword
