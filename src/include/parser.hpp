#pragma once

#include <memory>
#include <optional>
#include <expected>
#include <functional>
#include "ast.hpp"
#include "token.hpp"

namespace error {

class LexError;
class ErrorReporter;

} // namespace error

namespace parser::base {

class Parser {
public:
    Parser() = delete;
    explicit Parser(std::function<std::expected<lexer::token::Token, error::LexError>()> nextTokenFunc);
    ~Parser() = default;

public:
    [[nodiscard]] ast::ProgPtr parseProgram();

private:
    void advance();
    bool match(lexer::token::Type type);
    bool check(lexer::token::Type type) const;
    bool checkAhead(lexer::token::Type type);
    void expect(lexer::token::Type type, const std::string& error_msg);

    [[nodiscard]] ast::ArgPtr               parseArg();
    [[nodiscard]] ast::IfExprPtr            parseIfExpr();
    [[nodiscard]] ast::IfStmtPtr            parseIfStmt();
    [[nodiscard]] ast::ForStmtPtr           parseForStmt();
    [[nodiscard]] ast::RetStmtPtr           parseRetStmt();
    [[nodiscard]] ast::VarTypePtr           parseVarType();
    [[nodiscard]] ast::FuncDeclPtr          parseFuncDecl();
    [[nodiscard]] ast::CallExprPtr          parseCallExpr();
    [[nodiscard]] ast::LoopStmtPtr          parseLoopStmt();
    [[nodiscard]] ast::BlockStmtPtr         parseBlockStmt();
    [[nodiscard]] ast::BreakStmtPtr         parseBreakStmt();
    [[nodiscard]] ast::WhileStmtPtr         parseWhileStmt();
    [[nodiscard]] ast::ElseClausePtr        parseElseClause();
    [[nodiscard]] ast::NodePtr              parseStmtOrExpr();
    [[nodiscard]] ast::VarDeclStmtPtr       parseVarDeclStmt();
    [[nodiscard]] ast::AssignElementPtr     parseAssignElement();
    [[nodiscard]] ast::FuncHeaderDeclPtr    parseFuncHeaderDecl();
    [[nodiscard]] ast::FuncExprBlockStmtPtr parseFuncExprBlockStmt();
    [[nodiscard]] ast::AssignStmtPtr        parseAssignStmt(ast::AssignElementPtr&& lvalue);
    [[nodiscard]] ast::ExprPtr              parseExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    [[nodiscard]] ast::ExprPtr              parseFactor(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    [[nodiscard]] ast::ExprPtr              parseCmpExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    [[nodiscard]] ast::ExprPtr              parseAddExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    [[nodiscard]] ast::ExprPtr              parseMulExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    [[nodiscard]] ast::ExprPtr              parseElement(std::optional<ast::AssignElementPtr> elem = std::nullopt);

private:
    std::shared_ptr<error::ErrorReporter> reporter; // error reporter

    std::function<std::expected<lexer::token::Token, error::LexError>()> nextTokenFunc; // 获取下一个 token

    lexer::token::Token                current;   // 当前看到的 token
    std::optional<lexer::token::Token> lookahead; // 往后看一个 token
};

} // namespace parser::base
