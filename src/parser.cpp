#include <cassert>
#include "include/parser.hpp"

namespace parser::base {

/* constructor */

Parser::Parser(std::function<std::optional<lexer::token::Token>()> nextTokenFunc)
    : nextTokenFunc(std::move(nextTokenFunc)) {
    advance();
}

/* constructor */

/* member function definition */

/**
 * @brief 向前扫描一个 token
 */
void Parser::advance() {
    current = nextTokenFunc();
}

/**
 * @brief  匹配掉一个 token，并向前扫描一个 token
 * @param  type 需要匹配的 token 类型
 * @return 是否成功匹配
 */
bool Parser::match(lexer::token::Type type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

/**
 * @brief  检查当前看到的 token 是否存在，如果存在判断其类型是否为给定值
 * @param  type 指定的 token 类型
 * @return 是否通过检查
 */
bool Parser::check(lexer::token::Type type) const {
    return current.has_value() && current.value().getType() == type;
}

/**
 * @brief 匹配期望的 token，如果未匹配成功则抛出 runtime error
 * @param type      期望的 token 类型
 * @param error_msg 错误信息
 */
void Parser::expect(lexer::token::Type type, const std::string& error_msg) {
    if (!match(type)) {
        throw std::runtime_error{error_msg};
    }
}

/**
 * @brief  对指定程序进行语法解析
 * @return ast::ProgPtr - AST Program 结点指针 (AST 根结点)
 */
[[nodiscard]]
ast::ProgPtr Parser::parseProgram() {
    std::vector<ast::DeclPtr> decls; // declarations; Prog -> Decls & Decls -> Decl Decls | epsilon

    while (check(lexer::token::Type::FN)) {
        decls.push_back(parseFuncDecl());
    }

    return std::make_shared<ast::Prog>(std::move(decls));
}

/**
 * @brief  解析函数声明
 * @return ast::FuncDeclPtr - AST Function Declaration 结点指针
 */
[[nodiscard]]
ast::FuncDeclPtr Parser::parseFuncDecl() {
    // FuncDecl -> FuncHeaderDecl BlockStmt
    auto header = parseFuncHeaderDecl();
    auto body        = parseBlockStmt();
    return std::make_shared<ast::FuncDecl>(std::move(header), std::move(body));
}

/**
 * @brief  解析函数头声明
 * @return ast::FuncHeaderDeclPtr - AST Function Header Declaration 结点指针
 */
[[nodiscard]]
ast::FuncHeaderDeclPtr Parser::parseFuncHeaderDecl() {
    // FuncHeaderDecl -> fn <ID> ( <args> )
    using TokenType = lexer::token::Type;

    expect(TokenType::FN, "Expected 'fn'");

    expect(TokenType::ID, "Expected function name");
    std::string name = current->getValue(); // function name

    expect(TokenType::LPAREN, "Expected '('");
    std::vector<ast::ArgPtr> argv {};
    // while (!check(lexer::token::Type::RPAREN)) {
    //     expect(lexer::token::Type::ID, "Expected parameter name");
    //     argv.push_back(current->getValue());
    //     advance();
    //     if (match(TokenType::COMMA)) continue;
    //     break;
    // }
    expect(TokenType::RPAREN, "Expected ')'");

    return std::make_shared<ast::FuncHeaderDecl>(name, argv);
}

/**
 * @brief  解析语句块
 * @return ast::BlockStmtPtr - AST Block Statement 结点指针
 */
[[nodiscard]]
ast::BlockStmtPtr Parser::parseBlockStmt() {
    // BlockStmt -> { Stmts }
    // Stmts -> Stmt Stmts | epsilon
    using TokenType = lexer::token::Type;
    expect(TokenType::LBRACE, "Expected '{' for block");

    std::vector<ast::StmtPtr> stmts;
    while (!check(TokenType::RBRACE)) {
        if (check(TokenType::SEMICOLON)) {
            stmts.push_back(std::make_shared<ast::Stmt>());
            advance();
        } else {
            advance();
            break; //DEBUG
        }
    }

    expect(TokenType::RBRACE, "Expected '}' for block");
    return std::make_shared<ast::BlockStmt>(stmts);
}

/* member function definition */

} // namespace parser::base
