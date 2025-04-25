#pragma once

#include <optional>
#include <functional>
#include "ast.hpp"
#include "token.hpp"

namespace parser::base {

class Parser {
public:
    Parser() = delete;
    explicit Parser(std::function<std::optional<lexer::token::Token>()> nextTokenFunc);
    ~Parser() = default;

public:
    ast::ProgPtr parseProgram();

private:
    void advance();
    bool match(lexer::token::Type type);
    bool check(lexer::token::Type type) const;
    void expect(lexer::token::Type type, const std::string& error_msg);

    ast::FuncDeclPtr       parseFuncDecl();
    ast::FuncHeaderDeclPtr parseFuncHeaderDecl();
    ast::BlockStmtPtr      parseBlockStmt();

private:
    std::function<std::optional<lexer::token::Token>()> nextTokenFunc; // 获取下一个 token
    std::optional<lexer::token::Token>                  current;       // 当前看到的 token
};

} // namespace parser::base
