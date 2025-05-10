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
    ast::ProgPtr parseProgram();

private:
    void advance();
    bool match(lexer::token::Type type);
    bool check(lexer::token::Type type) const;
    bool checkAhead(lexer::token::Type type);
    void expect(lexer::token::Type type, const std::string& error_msg);

    ast::FuncDeclPtr          parseFuncDecl();
    ast::FuncHeaderDeclPtr    parseFuncHeaderDecl();
    ast::NodePtr              parseStmtOrExpr();
    ast::BlockStmtPtr         parseBlockStmt();
    ast::RetStmtPtr           parseRetStmt();
    ast::ArgPtr               parseArg();
    ast::VarDeclStmtPtr       parseVarDeclStmt();
    ast::AssignStmtPtr        parseAssignStmt(ast::AssignElementPtr&& lvalue);
    ast::AssignElementPtr     parseAssignElement();
    ast::ExprPtr              parseExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::ExprPtr              parseCmpExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::ExprPtr              parseAddExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::ExprPtr              parseMulExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::ExprPtr              parseFactor(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::ExprPtr              parseElementExpr(std::optional<ast::AssignElementPtr> elem = std::nullopt);
    ast::CallExprPtr          parseCallExpr();
    ast::IfStmtPtr            parseIfStmt();
    ast::ElseClausePtr        parseElseClause();
    ast::WhileStmtPtr         parseWhileStmt();
    ast::ForStmtPtr           parseForStmt();
    ast::LoopStmtPtr          parseLoopStmt();
    ast::VarTypePtr           parseVarType();
    ast::FuncExprBlockStmtPtr parseFuncExprBlockStmt();
    ast::IfExprPtr            parseIfExpr();
    ast::BreakStmtPtr         parseBreakStmt();

private:
    std::shared_ptr<error::ErrorReporter> reporter; // error reporter

    std::function<std::expected<lexer::token::Token, error::LexError>()> nextTokenFunc; // 获取下一个 token

    lexer::token::Token                current;   // 当前看到的 token
    std::optional<lexer::token::Token> lookahead; // 往后看一个 token
};

} // namespace parser::base
