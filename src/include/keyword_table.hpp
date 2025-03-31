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
        // 不能使用 this->keywords["IF"] = Token::IF 的形式插入
        // 因为我显式 delete 了 class Token 的默认构造函数，而
        // 上式会先调用默认构造函数构造一个 Token 对象，然后再拷贝
        // this->keywords.emplace("if", Token::IF);
    }

    ~KeywordTable() = default;

    inline bool isKeyword(std::string value) {
        return (keywords.find(value) != keywords.end());
    }

    inline void addKeyword(std::string name, Token token) {
        this->keywords.emplace(name, token);
    }

private:
    std::unordered_map<std::string, Token> keywords;
};

#endif
