#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include "token.hpp"
#include "lexer_position.hpp"

namespace lexer::base {

// abstract class lexer: define some utilities and data structure.
class Lexer {
public:
    Lexer() = delete;
    explicit Lexer(const std::vector<std::string>& t) : text(t) {}
    explicit Lexer(std::vector<std::string>&& t) : text(std::move(t)) {}
    virtual ~Lexer() = default;

public:
    virtual std::optional<token::Token> nextToken() = 0;

public:
    /**
     * @brief 重置当前扫描位置
     * @param pos 设置到的位置
     */
    inline void reset(const Position& p) {
        this->pos  = p;
        this->peek = this->text[p.row][p.col];
    }

protected:
    std::vector<std::string> text; // text to be scanned
    Position                 pos;  // the next position to be scanned
    char                     peek; // the next character to be scanned
};

} // namespace lexer::base
