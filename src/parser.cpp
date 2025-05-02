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
    if (lookahead.has_value()) {
        current = lookahead;
        lookahead.reset(); // 清除 lookahead 中的值
    } else {
        current = nextTokenFunc();
    }
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
 * @brief  向前检查一个 token，判断其类型是否为给定值
 * @param  type 指定的 token 类型
 * @return 是否通过检查
 */
bool Parser::checkAhead(lexer::token::Type type) {
    if (!lookahead.has_value()) {
        lookahead = nextTokenFunc(); // 获取下一个 token
    }
    return lookahead.has_value() && lookahead->getType() == type;
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
    while(!check(TokenType::RPAREN)) {
        argv.push_back(parseArg());
        if (!check(TokenType::COMMA)) {
            break;
        }
        advance();
    }

    expect(TokenType::RPAREN, "Expected ')'");

    if (check(TokenType::ARROW)) {
        expect(TokenType::ARROW, "Expected '->'");
        expect(TokenType::I32, "Expected 'i32'");

        return std::make_shared<ast::FuncHeaderDecl>(name, argv, TokenType::I32);
    }

    return std::make_shared<ast::FuncHeaderDecl>(name, argv, std::nullopt);
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

    std::vector<ast::StmtPtr> stmts {};
    while (!check(TokenType::RBRACE)) {
        if (check(TokenType::LET)) {
            stmts.push_back(parseVarDeclStmt());
        }
        else if (check(TokenType::RETURN)) {
            stmts.push_back(parseRetStmt());
        }
        else if (check(TokenType::ID) && checkAhead(TokenType::ASSIGN)) {
            stmts.push_back(parseAssignStmt());
        }
        else if (check(TokenType::INT) || check(TokenType::ID) || check(TokenType::LPAREN)){
            stmts.push_back(parseExpr());
        }
        //begin csx 5.2
        else if (check(TokenType::IF)){
            stmts.push_back(parseIfStmt());
        }
        else if (check(TokenType::WHILE)){
            stmts.push_back(parseWhileStmt());
        }
        else if (check(TokenType::FOR)){
            stmts.push_back(parseForStmt());
        }
        else if (check(TokenType::LOOP)){
            stmts.push_back(parseLoopStmt());
        }
        else if (check(TokenType::BREAK))
        {
            stmts.push_back(std::make_shared<ast::BreakStmt>());
            advance();
            expect(TokenType::SEMICOLON,"Expected ';' after Break");
        }
        else if (check(TokenType::CONTINUE))
        {
            stmts.push_back(std::make_shared<ast::ContinueStmt>());
            advance();
            expect(TokenType::SEMICOLON,"Expected ';' after Continue");
        }
        else if (check(TokenType::SEMICOLON)){
            stmts.push_back(std::make_shared<ast::NullStmt>());
            advance();
        }
    }

    expect(TokenType::RBRACE, "Expected '}' for block");
    return std::make_shared<ast::BlockStmt>(stmts);
}

/**
 * @brief  解析返回语句
 * @return ast::RetStmtPtr - AST Return Statement 结点指针
 */
[[nodiscard]]
ast::RetStmtPtr Parser::parseRetStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::RETURN, "Expected 'return'");

    if (check(TokenType::SEMICOLON)){
        expect(TokenType::SEMICOLON, "Expected ';'");
        return std::make_shared<ast::RetStmt>(std::nullopt);
    }

    ast::ExprPtr ret = Parser::parseCmpExpr();
    expect(TokenType::SEMICOLON, "Expected ';'");
    return std::make_shared<ast::RetStmt>(ret);
}

/**
 * @brief  解析参数
 * @return ast::ArgPtr - AST Argument 结点指针
 */
[[nodiscard]]
ast::ArgPtr Parser::parseArg() {
    using TokenType = lexer::token::Type;

    expect(TokenType::MUT, "Expected 'mut'");
    expect(TokenType::ID, "Expected '<ID>'");
    std::string name = current.value().getValue();

    expect(TokenType::COLON, "Expected ':'");
    expect(TokenType::I32, "Expected 'i32'");

    return std::make_shared<ast::Arg>(name);
}

/**
 * @brief  解析变量声明语句或变量声明赋值语句
 * @return ast::VarDeclStmtPtr - AST Variable Declaration Statement 结点指针
 */
[[nodiscard]]
ast::VarDeclStmtPtr Parser::parseVarDeclStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::LET, "Expected 'let'");
    expect(TokenType::MUT, "Expected 'mut'");
    expect(TokenType::ID, "Expected '<ID>'");
    lexer::token::Token identifier {current.value()};

    TokenType type;
    bool has_type = false;
    if (check(TokenType::COLON)) {
        has_type = true;
        expect(TokenType::COLON, "Expected ':'");
        expect(TokenType::I32, "Expected 'i32'");
        type = current.value().getType();
    }

    ast::ExprPtr expr;
    bool flag_assign = false;
    if (check(TokenType::ASSIGN)) {
        flag_assign = true;
        advance();
        expr = parseCmpExpr();
    }

    expect(TokenType::SEMICOLON, "Expected ';'");

    if (flag_assign) {
        std::make_shared<ast::VarDeclAssignStmt>(true, std::move(identifier),
        (has_type ? std::optional<TokenType>{type} : std::nullopt), expr);
    }
    return std::make_shared<ast::VarDeclStmt>(true, std::move(identifier),
        (has_type ? std::optional<TokenType>{type} : std::nullopt));
}

/**
 * @brief  解析赋值语句
 * @return ast::AssignStmtPtr - AST Assignment Statement 结点指针
 */
[[nodiscard]]
ast::AssignStmtPtr Parser::parseAssignStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::ID, "Expected '<ID>'");
    std::string name = current.value().getValue();

    expect(TokenType::ASSIGN, "Expected '='");
    ast::ExprPtr expr = Parser::parseCmpExpr();

    expect(TokenType::SEMICOLON, "Expected ';'");

    return std::make_shared<ast::AssignStmt>(name, expr);
}

/**
 * @brief  解析表达式，用递归下降解析分层处理运算优先级
 * @return  最顶层比较表达式
 */
[[nodiscard]]
ast::ExprPtr Parser::parseExpr() {
    using TokenType = lexer::token::Type;

    auto expr = Parser::parseCmpExpr();

    expect(TokenType::SEMICOLON, "Expected ';'");

    return expr;
}

/**
 * @brief  解析比较表达式（最顶层，即为Expr）
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的加法表达式）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseCmpExpr() {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = Parser::parseAddExpr();
    while (check(TokenType::OP_LT) || check(TokenType::OP_LE) ||
           check(TokenType::OP_GT) || check(TokenType::OP_GE) ||
           check(TokenType::OP_EQ) || check(TokenType::OP_NEQ)) {
        TokenType op_token = current->getType();
        advance();
        ast::ExprPtr right = Parser::parseAddExpr();
        left = std::make_shared<ast::ArithmeticExpr>(std::move(left),
                                                     op_token, std::move(right));
    } // end while

    return left;
}

/**
 * @brief  解析加法表达式
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的乘法表达式）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseAddExpr() {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = Parser::parseMulExpr();
    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        TokenType op_token = current->getType();
        advance();
        ast::ExprPtr right = Parser::parseMulExpr();
        left = std::make_shared<ast::ArithmeticExpr>(std::move(left),
                                                     op_token, std::move(right));
    }// end while

    return left;
}

/**
 * @brief  解析乘法表达式（即为Item）
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的因子）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseMulExpr() {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = Parser::parseFactorExpr();
    while (check(TokenType::OP_MUL) || check(TokenType::OP_DIV)) {
        TokenType op_token = current->getType();
        advance();
        ast::ExprPtr right = Parser::parseFactorExpr();
        left = std::make_shared<ast::ArithmeticExpr>(std::move(left),
                                                     op_token, std::move(right));
    } // end while

    return left;
}

/**
 * @brief  解析因子
 * @return ast:: - AST Expression 结点指针（3.1-2因子与元素相同，此处预留）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseFactorExpr() {
    return parseElementExpr();
}

/**
 * @brief  解析元素
 * @return ast::Number or ast::Variable - AST Expression 结点指针
 */
[[nodiscard]]
ast::ExprPtr Parser::parseElementExpr() {
    using TokenType = lexer::token::Type;

    if (check(TokenType::LPAREN)) {
        advance();
        ast::ExprPtr expr = Parser::parseCmpExpr();
        expect(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    if (check(TokenType::INT)) {
        int value = std::stoi(current->getValue());
        advance();
        return std::make_shared<ast::Number>(value);
    } else if (check(TokenType::ID)) {
        if (checkAhead(TokenType::LPAREN)) {
            return parseCallExpr();
        } else {
            std::string name = current->getValue();
            advance();
            return std::make_shared<ast::Variable>(name);
        }
    } else {
        throw std::runtime_error("Unexpected token in expression: " + current->getValue());
    }
}

/**
 * @brief  解析函数调用
 * @return ast::CallExpr - AST Expression 结点指针
 */
[[nodiscard]]
ast::CallExprPtr Parser::parseCallExpr() {
    using TokenType = lexer::token::Type;

    expect(TokenType::ID, "Expected function name");
    std::string name = current->getValue(); // function name
    expect(TokenType::LPAREN, "Expected '('");

    std::vector<ast::ExprPtr> argv{};
    while(!check(TokenType::RPAREN)) {
        argv.push_back(parseCmpExpr());
        if (!check(TokenType::COMMA)) {
            break;
        }
        advance();
    }

    expect(TokenType::RPAREN, "Expected ')'");
    return std::make_shared<ast::CallExpr>(name, argv);
}

/**
 * @brief  解析 if 语句
 * @return ast::IfStmtPtr - AST If Statement 结点指针
 */
ast::IfStmtPtr Parser::parseIfStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::IF, "Expected 'if'");
    auto expr   = parseCmpExpr();
    auto block  = parseBlockStmt();

    std::vector<ast::ElseClausePtr> elcls{};
    while(check(TokenType::ELSE)) {
        advance();
        bool end = !check(TokenType::IF);
        elcls.push_back(parseElseClause());
        if (end) {
            break;
        }
    }

    return std::make_shared<ast::IfStmt>(std::move(expr),
        std::move(block), std::move(elcls));
}


/**
 * @brief 解析Else/Else if 语句
 * @return ast::ElseClausePtr - AST ELSE Statement 结点指针
 */
ast::ElseClausePtr Parser::parseElseClause() {
    using TokenType = lexer::token::Type;

    if(check(TokenType::IF)) {
        advance();
        auto expr = parseCmpExpr();
        auto block = parseBlockStmt();
        return std::make_shared<ast::ElseClause>(std::move(expr), std::move(block));
    } else {
        auto block = parseBlockStmt();
        return std::make_shared<ast::ElseClause>(std::nullopt, std::move(block));
    }
}

/**
 * @brief 解析While语句
 * @return ast::WhileStmtPtr - AST While Statement 结点指针
 */
ast::WhileStmtPtr Parser::parseWhileStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::WHILE,"Expected 'while'");

    auto expr = parseCmpExpr();
    auto block = parseBlockStmt();

    return std::make_shared<ast::WhileStmt>(std::move(expr), std::move(block));
}

/**
 * @brief 解析For语句
 * @return ast::ForStmtPtr - AST For Statement 结点指针
 */
ast::ForStmtPtr Parser::parseForStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::FOR,"Expected 'for'");
    expect(TokenType::MUT, "Expected 'mut'");
    expect(TokenType::ID, "Expected '<ID>'");
    lexer::token::Token identifier {current.value()};

    expect(TokenType::IN,"Expected 'in'");

    auto expr1 = parseCmpExpr();
    expect(TokenType::DOTS,"Expected '..'");

    auto expr2 = parseCmpExpr();
    auto block = parseBlockStmt();

    return std::make_shared<ast::ForStmt>(std::move(identifier.getValue()),
        std::move(expr1), std::move(expr2), std::move(block));
}

/**
 * @brief 解析Loop语句
 * @return ast::LoopStmtPtr - AST Loop Statement 结点指针
 */
ast::LoopStmtPtr Parser::parseLoopStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::LOOP,"Expected 'loop'");
    auto block = parseBlockStmt();

    return std::make_shared<ast::LoopStmt>(std::move(block));
}

/* member function definition */

} // namespace parser::base
