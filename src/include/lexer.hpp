#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <expected>
#include "token.hpp"
#include "lexer_position.hpp"

namespace error {

class LexError;
class ErrorReporter;

} // namespace error

namespace lexer::base {

// abstract class lexer: define some utilities and data structure.
class Lexer {
public:
    Lexer() = delete;
    explicit Lexer(const std::vector<std::string>& t) : text(t) {}
    explicit Lexer(std::vector<std::string>&& t) : text(std::move(t)) {}
    virtual ~Lexer() = default;

public:
    virtual auto nextToken() -> std::expected<token::Token, error::LexError> = 0;

public:
    /**
     * @brief 重置当前扫描位置
     * @param pos 设置到的位置
     */
    inline void reset(const Position& p) {
        this->pos  = p;
        this->peek = this->text[p.row][p.col];
    }

    /**
     * @brief 设置错误报告器
     * @param reporter 全局错误报告器
     */
    inline void setErrReporter(std::shared_ptr<error::ErrorReporter> reporter) {
        this->reporter = std::move(reporter);
    }

protected:
    std::shared_ptr<error::ErrorReporter> reporter; // error reporter

    std::vector<std::string>       text; // text to be scanned
    Position                       pos;  // the next position to be scanned
    char                           peek; // the next character to be scanned
};

} // namespace lexer::base
