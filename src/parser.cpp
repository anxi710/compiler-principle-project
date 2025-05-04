#include <cassert>
#include "include/parser.hpp"
#include<iostream>

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
        if (lookahead2.has_value()) {
            lookahead = lookahead2;
            lookahead2.reset();
        }
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
 * @brief  向前检查两个 token，判断对应类型是否为对应值
 * @param  type1 前方第一个 token 类型
 * @param  type2 前方第二个 token 类型
 * @return 是否通过检查
 */
bool Parser::checkAhead2(lexer::token::Type type1, lexer::token::Type type2) {
    if (!lookahead.has_value()) {
        lookahead = nextTokenFunc();
        if (!lookahead.has_value()) {
            return false;
        }
    }
    if (!lookahead2.has_value()) {
        lookahead2 = nextTokenFunc();
        if (!lookahead2.has_value()) {
            return false;
        }
    }
    return lookahead->getType() == type1 && lookahead2->getType() == type2;
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
        auto type = parseVarType();
        return std::make_shared<ast::FuncHeaderDecl>(std::move(name), std::move(argv), std::move(type));
    }
    return std::make_shared<ast::FuncHeaderDecl>(std::move(name), std::move(argv), std::nullopt);
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
    ast::ExprPtr expr;
    bool flag_func_expr = false;
    while (!check(TokenType::RBRACE)) {
        ast::NodePtr node = parseStmtOrExpr();
        if (auto stmt = std::dynamic_pointer_cast<ast::Stmt>(node)) {
            stmts.push_back(stmt);
            continue;
        }
        expr = std::dynamic_pointer_cast<ast::Expr>(node);
        if (nullptr != expr) {
            if (check(TokenType::SEMICOLON)) {
                advance();
                stmts.push_back(std::make_shared<ast::ExprStmt>(std::move(expr)));
                continue;
            } else {
                flag_func_expr = true;
                break;
            }
        }
    }

    expect(TokenType::RBRACE, "Expected '}' for block");
    if (flag_func_expr) {
        return std::make_shared<ast::FuncExprBlockStmt>(std::move(stmts), std::move(expr));
    }
    return std::make_shared<ast::BlockStmt>(std::move(stmts));
}

/**
 * @brief  解析语句或表达式
 * @return ast::NodePtr - Stmt 或 Expr 结点指针
 */
[[nodiscard]]
ast::NodePtr Parser::parseStmtOrExpr() {
    using TokenType = lexer::token::Type;

    ast::StmtPtr stmt {};
    if (check(TokenType::LET)) {
        stmt = parseVarDeclStmt();
    } else if (check(TokenType::RETURN)) {
        stmt = parseRetStmt();
    } else if (check(TokenType::ID) && checkAhead(TokenType::ASSIGN)) {
        stmt = parseAssignStmt();
    } else if (check(TokenType::INT) || check(TokenType::ID) || check(TokenType::LPAREN)) {
        return parseExpr();
    } else if (check(TokenType::IF)) {
        stmt = parseIfStmt();
    } else if (check(TokenType::WHILE)) {
        stmt = parseWhileStmt();
    } else if (check(TokenType::FOR)) {
        stmt = parseForStmt();
    } else if (check(TokenType::LOOP)) {
        stmt = parseLoopStmt();
    } else if (check(TokenType::OP_MUL)) {
        if (checkAhead2(TokenType::ID, TokenType::ASSIGN)) {
            stmt = parseAssignStmt();
        } else if (checkAhead(TokenType::ID)) {
            return parseExpr();
        }
    } else if (check(TokenType::BREAK)) {
        stmt = parseBreakStmt();
    } else if (check(TokenType::CONTINUE)) {
        stmt = std::make_shared<ast::ContinueStmt>();
        advance();
        expect(TokenType::SEMICOLON,"Expected ';' after Continue");
    } else if (check(TokenType::SEMICOLON)){
        stmt = std::make_shared<ast::NullStmt>();
        advance();
    }

    return stmt;
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
    return std::make_shared<ast::RetStmt>(std::move(ret));
}

/**
 * @brief  解析参数
 * @return ast::ArgPtr - AST Argument 结点指针
 */
[[nodiscard]]
ast::ArgPtr Parser::parseArg() {
    using TokenType = lexer::token::Type;

    bool mutable_ = false;
    if (check(TokenType::MUT)) {
        mutable_ = true;
        advance();
    }
    expect(TokenType::ID, "Expected '<ID>'");
    auto var = std::make_shared<ast::VarDecl>(mutable_, current->getValue());

    expect(TokenType::COLON, "Expected ':'");
    auto type = parseVarType();

    return std::make_shared<ast::Arg>(std::move(var), std::move(type));
}

/**
 * @brief  解析变量声明语句或变量声明赋值语句
 * @return ast::VarDeclStmtPtr - AST Variable Declaration Statement 结点指针
 */
[[nodiscard]]
ast::VarDeclStmtPtr Parser::parseVarDeclStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::LET, "Expected 'let'");

    bool mutable_ = false;
    if (check(TokenType::MUT)) {
        mutable_ = true;
        advance();
    }
    expect(TokenType::ID, "Expected '<ID>'");
    auto identifier  = std::make_shared<ast::VarDecl>(mutable_, current->getValue());

    ast::VarTypePtr type;
    bool            has_type = false;
    if (check(TokenType::COLON)) {
        has_type = true;
        advance();
        type = parseVarType();
    }

    ast::ExprPtr expr {};
    bool flag_assign = false;
    if (check(TokenType::ASSIGN)) {
        flag_assign = true;
        advance();
        expr = parseExpr();
    }

    expect(TokenType::SEMICOLON, "Expected ';'");

    if (flag_assign) {
        return std::make_shared<ast::VarDeclAssignStmt>(std::move(identifier),
        (has_type ? std::optional<ast::VarTypePtr>{type} : std::nullopt), std::move(expr));
    }
    return std::make_shared<ast::VarDeclStmt>(std::move(identifier),
        (has_type ? std::optional<ast::VarTypePtr>{type} : std::nullopt));
}

/**
 * @brief  解析赋值语句
 * @return ast::AssignStmtPtr - AST Assignment Statement 结点指针
 */
[[nodiscard]]
ast::AssignStmtPtr Parser::parseAssignStmt() {
    using TokenType = lexer::token::Type;

    ast::RefType type {ast::RefType::Normal};
    if (check(TokenType::OP_MUL)) {
        advance();
        type = ast::RefType::Deref;
    }
    expect(TokenType::ID, "Expected '<ID>'");
    std::string var = current.value().getValue();

    expect(TokenType::ASSIGN, "Expected '='");
    ast::ExprPtr expr = Parser::parseCmpExpr();

    expect(TokenType::SEMICOLON, "Expected ';'");

    return std::make_shared<ast::AssignStmt>(std::move(var), type, std::move(expr));
}

/**
 * @brief  解析表达式，用递归下降解析分层处理运算优先级
 * @return ast::ExprPtr - 顶层比较表达式
 */
[[nodiscard]]
ast::ExprPtr Parser::parseExpr() {
    using TokenType = lexer::token::Type;

    if (check(TokenType::LBRACE)) {
        return parseFuncExprBlockStmt();
    } else if (check(TokenType::IF)) {
        return  parseIfExpr();
    } else if (check(TokenType::LOOP)) {
        return parseLoopStmt();
    } else {
        return Parser::parseCmpExpr();
    }
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
 * @return ast::FactorPtr - AST Factor 结点指针
 */
[[nodiscard]]
ast::ExprPtr Parser::parseFactorExpr() {
    using TokenType = lexer::token::Type;

    // arrayElements
    if (check(TokenType::LBRACK)){
        advance();
        std::vector<ast::ExprPtr> elements{};
        while(!check(TokenType::RBRACK))
        {
            elements.push_back(parseExpr());
            if (!check(TokenType::COMMA))
                break;
            advance();
        }
        advance();
        return std::make_shared<ast::ArrayElements>(elements);
    }
    
    ast::RefType ref_type {ast::RefType::Normal};
    if (check(TokenType::OP_MUL)) {
        advance();
        ref_type = ast::RefType::Deref;
    } else if (check(TokenType::Ref)) {
        advance();
        if (check(TokenType::MUT)) {
            advance();
            ref_type = ast::RefType::Mutable;
        } else {
            ref_type = ast::RefType::Immutable;
        }
    }

    auto element = parseElementExpr();

    return std::make_shared<ast::Factor>(ref_type, std::move(element));
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
            return std::make_shared<ast::Variable>(std::move(name));
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
    return std::make_shared<ast::CallExpr>(std::move(name), std::move(argv));
}

/**
 * @brief  解析 if 语句
 * @return ast::IfStmtPtr - AST If Statement 结点指针
 */
[[nodiscard]]
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
 * @brief  解析 else/else if 语句
 * @return ast::ElseClausePtr - AST ELSE Statement 结点指针
 */
[[nodiscard]]
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
 * @brief  解析 while 语句
 * @return ast::WhileStmtPtr - AST While Statement 结点指针
 */
[[nodiscard]]
ast::WhileStmtPtr Parser::parseWhileStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::WHILE,"Expected 'while'");

    auto expr = parseCmpExpr();
    auto block = parseBlockStmt();

    return std::make_shared<ast::WhileStmt>(std::move(expr), std::move(block));
}

/**
 * @brief  解析 for 语句
 * @return ast::ForStmtPtr - AST For Statement 结点指针
 */
[[nodiscard]]
ast::ForStmtPtr Parser::parseForStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::FOR,"Expected 'for'");

    bool mutable_ = false;
    if (check(TokenType::MUT)) {
        mutable_ = true;
        advance();
    }
    expect(TokenType::ID, "Expected '<ID>'");
    ast::VarDeclPtr var = std::make_shared<ast::VarDecl>(mutable_, current->getValue());

    expect(TokenType::IN,"Expected 'in'");

    auto expr1 = parseCmpExpr();
    expect(TokenType::DOTS,"Expected '..'");

    auto expr2 = parseCmpExpr();
    auto block = parseBlockStmt();

    return std::make_shared<ast::ForStmt>(std::move(var),
        std::move(expr1), std::move(expr2), std::move(block));
}

/**
 * @brief  解析Loop语句
 * @return ast::LoopStmtPtr - AST Loop Statement 结点指针
 */
[[nodiscard]]
ast::LoopStmtPtr Parser::parseLoopStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::LOOP,"Expected 'loop'");
    auto block = parseBlockStmt();

    return std::make_shared<ast::LoopStmt>(std::move(block));
}

/**
 * @brief  解析变量类型
 * @return ast::VarTypePtr - AST Variable Type 结点指针
 */
[[nodiscard]]
ast::VarTypePtr Parser::parseVarType() {
    using TokenType = lexer::token::Type;

    ast::RefType ref_type {ast::RefType::Normal};
    if (check(TokenType::Ref)) {
        advance();
        if (check(TokenType::MUT)) {
            advance();
            ref_type = ast::RefType::Mutable;
        } else {
            ref_type = ast::RefType::Immutable;
        }
    }

    if (check(TokenType::LBRACK)){
        advance();
        ast::VarTypePtr elementType = parseVarType();
        expect(TokenType::SEMICOLON,"Expected ';' for Array");
        int cnt =std::stoi(current->getValue());
        expect(TokenType::INT,"Expected <NUM> for Array");
        expect(TokenType::RBRACK,"Expected ']' for Array");
        return std::make_shared<ast::Array>(cnt,elementType,ref_type);
    }

    if (check(TokenType::I32)) {
        advance();
        return std::make_shared<ast::Integer>(ref_type);
    }

    throw std::runtime_error{"Incorrect variable type"};
}

/**
 * @brief  解析函数表达式语句块
 * @return ast::FuncExprBlockStmtPtr - AST Function Expression Block Statements 结点指针
 */
[[nodiscard]]
ast::FuncExprBlockStmtPtr Parser::parseFuncExprBlockStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::LBRACE, "Expected '{' for function expression block statements");

    std::vector<ast::StmtPtr> stmts {};
    ast::ExprPtr              expr  {};
    while (!check(TokenType::RBRACE)) {
        ast::NodePtr node = parseStmtOrExpr();
        if (auto stmt = std::dynamic_pointer_cast<ast::Stmt>(node)) {
            stmts.push_back(stmt);
            continue;
        }
        expr = std::dynamic_pointer_cast<ast::Expr>(node);
        if (nullptr != expr) {
            if (check(TokenType::SEMICOLON)) {
                advance();
                stmts.push_back(std::make_shared<ast::ExprStmt>(std::move(expr)));
                continue;
            } else {
                break;
            }
        }
    }

    expect(TokenType::RBRACE, "Expected '}' for block");

    return std::make_shared<ast::FuncExprBlockStmt>(std::move(stmts), std::move(expr));;
}

/**
 * @brief  解析 if 表达式
 * @return ast::IfExprPtr - AST If Expression 结点指针
 */
[[nodiscard]]
ast::IfExprPtr Parser::parseIfExpr() {
    using TokenType = lexer::token::Type;

    expect(TokenType::IF, "Expected 'if' for If Expression");

    auto condition = parseExpr();
    auto if_branch = parseFuncExprBlockStmt();

    expect(TokenType::ELSE, "Expected 'else' for If expression");
    auto else_branch = parseFuncExprBlockStmt();

    return std::make_shared<ast::IfExpr>(std::move(condition), std::move(if_branch), std::move(else_branch));
}

/**
 * @brief  解析 Break 表达式
 * @return ast::BreakStmtPtr - AST Break Statement 结点指针
 */
[[nodiscard]]
ast::BreakStmtPtr Parser::parseBreakStmt() {
    using TokenType = lexer::token::Type;

    expect(TokenType::BREAK, "Expected 'break' for break statement");

    if (!check(TokenType::SEMICOLON)) {
        auto expr = parseExpr();
        return std::make_shared<ast::BreakStmt>(std::move(expr));
    }

    return std::make_shared<ast::BreakStmt>();
}

/* member function definition */

} // namespace parser::base
