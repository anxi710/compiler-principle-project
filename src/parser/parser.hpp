#pragma once

#include <expected>
#include <functional>
#include <memory>
#include <optional>

#include "../lexer/token.hpp"
#include "ast.hpp"

namespace error
{

struct LexError;
class ErrorReporter;

}  // namespace error

namespace parser::base
{

class Parser
{
   public:
    Parser() = delete;
    explicit Parser(
        std::function<std::expected<lexer::token::Token, error::LexError>()> nextTokenFunc);
    ~Parser() = default;

   public:
    [[nodiscard]] auto parseProgram() -> ast::ProgPtr;

   private:
    void advance();
    auto match(lexer::token::Type type) -> bool;
    [[nodiscard]] auto check(lexer::token::Type type) const -> bool;
    auto checkAhead(lexer::token::Type type) -> bool;
    void expect(lexer::token::Type type, const std::string& error_msg);

    [[nodiscard]] auto parseArg() -> ast::ArgPtr;
    [[nodiscard]] auto parseIfExpr() -> ast::IfExprPtr;
    [[nodiscard]] auto parseIfStmt() -> ast::IfStmtPtr;
    [[nodiscard]] auto parseForStmt() -> ast::ForStmtPtr;
    [[nodiscard]] auto parseRetStmt() -> ast::RetStmtPtr;
    [[nodiscard]] auto parseVarType() -> ast::VarTypePtr;
    [[nodiscard]] auto parseFuncDecl() -> ast::FuncDeclPtr;
    [[nodiscard]] auto parseCallExpr() -> ast::CallExprPtr;
    [[nodiscard]] auto parseLoopStmt() -> ast::LoopStmtPtr;
    [[nodiscard]] auto parseBlockStmt() -> ast::BlockStmtPtr;
    [[nodiscard]] auto parseBreakStmt() -> ast::BreakStmtPtr;
    [[nodiscard]] auto parseWhileStmt() -> ast::WhileStmtPtr;
    [[nodiscard]] auto parseElseClause() -> ast::ElseClausePtr;
    [[nodiscard]] auto parseStmtOrExpr() -> ast::NodePtr;
    [[nodiscard]] auto parseVarDeclStmt() -> ast::VarDeclStmtPtr;
    [[nodiscard]] auto parseAssignElement() -> ast::AssignElementPtr;
    [[nodiscard]] auto parseFuncHeaderDecl() -> ast::FuncHeaderDeclPtr;
    [[nodiscard]] auto parseFuncExprBlockStmt() -> ast::FuncExprBlockStmtPtr;
    [[nodiscard]] auto parseAssignStmt(ast::AssignElementPtr&& lvalue) -> ast::AssignStmtPtr;

    [[nodiscard]] auto parseExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;
    [[nodiscard]] auto parseFactor(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;
    [[nodiscard]] auto parseCmpExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;
    [[nodiscard]] auto parseAddExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;
    [[nodiscard]] auto parseMulExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;
    [[nodiscard]] auto parseElement(std::optional<ast::AssignElementPtr> elem = std::nullopt)
        -> ast::ExprPtr;

   private:
    std::shared_ptr<error::ErrorReporter> reporter;  // error reporter

    std::function<std::expected<lexer::token::Token, error::LexError>()>
        nextTokenFunc;  // 获取下一个 token

    lexer::token::Token current;                   // 当前看到的 token
    std::optional<lexer::token::Token> lookahead;  // 往后看一个 token
};

}  // namespace parser::base
