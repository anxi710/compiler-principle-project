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
    ast::ExprPtr              parseElement(std::optional<ast::AssignElementPtr> elem = std::nullopt);
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
    std::function<std::optional<lexer::token::Token>()> nextTokenFunc; // 获取下一个 token
    std::optional<lexer::token::Token>                  current;       // 当前看到的 token
    std::optional<lexer::token::Token>                  lookahead;     // 往后看一个 token
};

} // namespace parser::base
