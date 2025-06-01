#pragma once

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "../err_report/error_reporter.hpp"
#include "token_type.hpp"

namespace lexer::keyword
{

/**
 * @brief   关键字表
 * @details 内置一个存储所有关键字的 hash map，用于查找判断指定 token 是否为关键字
 */
class KeywordTable
{
   public:
    KeywordTable() = default;
    ~KeywordTable() = default;

   public:
    /**
     * @brief  判断给定的输入值是否是一个关键字
     * @param  v token value
     * @return true / false
     */
    auto iskeyword(const std::string& v) const -> bool { return (keywords.contains(v)); }

    /**
     * @brief  根据输入值获取对应 token type
     * @param  v token value
     * @return keyword token type
     */
    auto getKeyword(const std::string& v) const -> token::Type
    {
        std::ostringstream oss;
        if (!keywords.contains(v))
        {
            oss << "调用参数（" << v << "）并非关键字";
            reporter->report(error::InternalErrorType::UnknownKeyword, oss.str());
        }
        return keywords.find(v)->second;
    }

    /**
     * @brief 向关键词表中添加一个关键词类型
     * @param n keyword name
     * @param t keyword token type
     */
    void addKeyword(std::string n, token::Type t) { this->keywords.emplace(n, t); }

    /**
     * @brief 设置错误报告器
     * @param reporter 全局错误报告器
     */
    void setErrReporter(std::shared_ptr<error::ErrorReporter> reporter)
    {
        this->reporter = std::move(reporter);
    }

   private:
    std::shared_ptr<error::ErrorReporter> reporter;         // error reporter
    std::unordered_map<std::string, token::Type> keywords;  // keyword hash map
};

}  // namespace lexer::keyword
