#include <cassert>
#include "include/parser.hpp"
#include "include/error_reporter.hpp"

namespace parser::base {

/* constructor */

Parser::Parser(std::function<std::expected<lexer::token::Token, error::LexError>()> nextTokenFunc)
    : nextTokenFunc(std::move(nextTokenFunc)) {
    advance(); // 初始化，使 current 指向第一个 token
}

/* constructor */

/* member function definition */

/**
 * @brief 向前扫描一个 token
 */
void Parser::advance() {
    if (lookahead.has_value()) {
        current = lookahead.value();
        lookahead.reset(); // 清除 lookahead 中的值
    } else {
        if (auto token = nextTokenFunc();
            token.has_value()) {
            current = token.value();
        } else { // 如果识别到未知 token，则发生了词法分析错误，且需要立即终止
            reporter->report(token.error(), true);
        }
    }
}

/**
 * @brief  匹配当前 token，并向前扫描一个 token
 * @param  type 需匹配的 token 类型
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
    return current.getType() == type;
}

/**
 * @brief  向前检查一个 token，判断其类型是否为给定值
 * @param  type 指定的 token 类型
 * @return 是否通过检查
 */
bool Parser::checkAhead(lexer::token::Type type) {
    if (!lookahead.has_value()) {
        if (auto token = nextTokenFunc();
            token.has_value()) {
            lookahead = token.value(); // 获取下一个 token
        } else { // 如果识别到未知 token，则发生了词法分析错误，且需要立即终止
            reporter->report(token.error(), true);
        }
    }
    return lookahead.has_value() && lookahead->getType() == type;
}

/**
 * @brief 匹配期望的 token，如果未匹配成功则抛出 runtime error
 * @param type 期望的 token 类型
 * @param msg  错误信息
 */
void Parser::expect(lexer::token::Type type, const std::string& msg) {
    if (!match(type)) {
        reporter->report(
            error::ParseErrorType::UnexpectToken,
            msg,
            current.getPos().row,
            current.getPos().col,
            current.getValue()
        );
    }
}

/**
 * @brief  对指定程序进行语法解析
 * @return ast::ProgPtr - AST Program 结点指针 (AST 根结点)
 */
[[nodiscard]]
ast::ProgPtr Parser::parseProgram() {
    std::vector<ast::DeclPtr> decls; // declarations;

    // Prog -> (FuncDecl)*
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
    // FuncHeaderDecl -> fn <ID> ( (arg)* ) (-> VarType)?
    using TokenType = lexer::token::Type;

    expect(TokenType::FN, "此处期望有一个 'fn'");

    std::string name = current.getValue(); // function name
    expect(TokenType::ID, "此处期望有一个 '<ID>' 作为函数名");

    expect(TokenType::LPAREN, "此处期望有一个 '('");

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
    // BlockStmt -> { (Stmt)* }; FuncExprBlockStmt -> { (Stmt)* Expr }
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
    } else if (check(TokenType::ID) || check(TokenType::OP_MUL)) {
        if (check(TokenType::ID) && checkAhead(TokenType::LPAREN)) {
            return parseCallExpr();
        }
        /*
         * x, *x, x[idx], x.idx 都即可以作为赋值语句的左值，又可以作为表达式的一个操作数
         */
        auto elem = parseAssignElement();
        if (check(TokenType::ASSIGN)) {
            stmt =  parseAssignStmt(std::move(elem));
        } else {
            return parseExpr(elem);
        }
    } else if (check(TokenType::INT) || check(TokenType::LPAREN)) {
        return parseExpr();
    } else if (check(TokenType::IF)) {
        stmt = parseIfStmt();
    } else if (check(TokenType::WHILE)) {
        stmt = parseWhileStmt();
    } else if (check(TokenType::FOR)) {
        stmt = parseForStmt();
    } else if (check(TokenType::LOOP)) {
        stmt = parseLoopStmt();
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

    if (check(TokenType::SEMICOLON)) {
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
    std::string name = current.getValue();
    expect(TokenType::ID, "Expected '<ID>'");
    auto var = std::make_shared<ast::VarDeclBody>(mutable_, name);

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

    bool mut = false;
    if (check(TokenType::MUT)) {
        mut = true;
        advance();
    }
    expect(TokenType::ID, "Expected '<ID>'");
    auto identifier  = std::make_shared<ast::VarDeclBody>(mut, current.getValue());

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
ast::AssignStmtPtr Parser::parseAssignStmt(ast::AssignElementPtr&& lvalue) {
    using TokenType = lexer::token::Type;

    expect(TokenType::ASSIGN, "Expected '='");
    ast::ExprPtr expr = Parser::parseExpr();

    expect(TokenType::SEMICOLON, "Expected ';'");

    return std::make_shared<ast::AssignStmt>(std::move(lvalue),std::move(expr));
}

/**
 * @brief 解析可赋值元素
 * @return ast::AssignElementPtr - AST Assign Element 节点指针
 */
ast::AssignElementPtr Parser::parseAssignElement() {
    using TokenType = lexer::token::Type;

    if (check(TokenType::OP_MUL)) {
        advance();
        expect(TokenType::ID, "Expected '<ID>'");
        std::string var = current.getValue();
        return std::make_shared<ast::Dereference>(std::move(var));
    }

    expect(TokenType::ID, "Expected '<ID>'");
    std::string var = current.getValue();
    if(check(TokenType::LBRACK)) {
        advance();
        auto expr = parseExpr();
        expect(TokenType::RBRACK,"Expected ']'");
        return std::make_shared<ast::ArrayAccess>(std::move(var),std::move(expr));
    }

    if (check(TokenType::DOT)) {
        advance();
        int value = std::stoi(current.getValue());
        expect(TokenType::INT, "Expected <NUM> for Tuple");
        return std::make_shared<ast::TupleAccess>(std::move(var), std::move(value));
    }

    return std::make_shared<ast::Variable>(std::move(var));
}

/**
 * @brief  解析表达式，用递归下降解析分层处理运算优先级
 * @return ast::ExprPtr - 顶层比较表达式
 */
[[nodiscard]]
ast::ExprPtr Parser::parseExpr(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    if (check(TokenType::LBRACE)) {
        return parseFuncExprBlockStmt();
    } else if (check(TokenType::IF)) {
        return  parseIfExpr();
    } else if (check(TokenType::LOOP)) {
        return parseLoopStmt();
    } else {
        return Parser::parseCmpExpr(elem);
    }
}

/**
 * @brief  将 token type 转换为 comparison operator
 * @param  t token type
 * @return comparison operator
 */
static ast::ComparOperator tokenType2ComparOper(lexer::token::Type t) {
    using TokenType = lexer::token::Type;
    using CmpOper   = ast::ComparOperator;
    static std::unordered_map<TokenType, CmpOper> map {
        {TokenType::OP_EQ,  CmpOper::Equal},
        {TokenType::OP_NEQ, CmpOper::Nequal},
        {TokenType::OP_GE,  CmpOper::Gequal},
        {TokenType::OP_GT,  CmpOper::Great},
        {TokenType::OP_LT,  CmpOper::Less}
    };

    if (auto res = map.find(t);
        res == map.end()) {
        throw std::runtime_error{"Incorrect token type."};
    } else {
        return res->second;
    }
}

/**
 * @brief  将 token type 转换为 arithmetic operator
 * @param  t token type
 * @return arithmetic operator
 */
static ast::ArithOperator tokenType2ArithOper(lexer::token::Type t) {
    using TokenType = lexer::token::Type;
    static std::unordered_map<TokenType, ast::ArithOperator> map {
        {TokenType::OP_PLUS,  ast::ArithOperator::Add},
        {TokenType::OP_MINUS, ast::ArithOperator::Sub},
        {TokenType::OP_MUL,   ast::ArithOperator::Mul},
        {TokenType::OP_DIV,   ast::ArithOperator::Div}
    };

    if (auto res = map.find(t);
        res == map.end()) {
        throw std::runtime_error{"Incorrect token type."};
    } else {
        return res->second;
    }
}

/**
 * @brief  解析比较表达式（最顶层的表达式）
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的加法表达式）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseCmpExpr(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = Parser::parseAddExpr(elem);
    while (check(TokenType::OP_LT) || check(TokenType::OP_LE) ||
           check(TokenType::OP_GT) || check(TokenType::OP_GE) ||
           check(TokenType::OP_EQ) || check(TokenType::OP_NEQ)) {
        TokenType op = current.getType();
        advance();

        ast::ExprPtr right = Parser::parseAddExpr();

        left = std::make_shared<ast::ComparExpr>(
            std::move(left),
            tokenType2ComparOper(op),
            std::move(right)
        );
    } // end while

    return left;
}

/**
 * @brief  解析加法表达式
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的乘法表达式）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseAddExpr(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = Parser::parseMulExpr(elem);
    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        TokenType op = current.getType();
        advance();

        ast::ExprPtr right = Parser::parseMulExpr();

        left = std::make_shared<ast::ArithExpr>(
            std::move(left),
            tokenType2ArithOper(op),
            std::move(right)
        );
    }// end while

    return left;
}

/**
 * @brief  解析乘法表达式（即为Item）
 * @return ast::ArithmeticExprPtr - AST Expression 结点指针（若无，则为下一层的因子）
 */
[[nodiscard]]
ast::ExprPtr Parser::parseMulExpr(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    ast::ExprPtr left = parseFactor(elem);
    while (check(TokenType::OP_MUL) || check(TokenType::OP_DIV)) {
        TokenType op = current.getType();
        advance();

        ast::ExprPtr right = Parser::parseFactor();

        left = std::make_shared<ast::ArithExpr>(
            std::move(left),
            tokenType2ArithOper(op),
            std::move(right)
        );
    } // end while

    return left;
}

/**
 * @brief  解析因子
 * @return ast::FactorPtr - AST Factor 结点指针
 */
[[nodiscard]]
ast::ExprPtr Parser::parseFactor(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    // ArrayElements
    if (check(TokenType::LBRACK)) {
        advance();
        std::vector<ast::ExprPtr> elements{};
        while(!check(TokenType::RBRACK)) {
            elements.push_back(parseExpr());
            if (!check(TokenType::COMMA))
                break;
            advance();
        }
        advance();
        return std::make_shared<ast::ArrayElements>(elements);
    }

    // TupleElements
    if (check(TokenType::LPAREN)) {
        advance();
        std::vector<ast::ExprPtr> elems{};

        while (!check(TokenType::RPAREN)) {
            elems.push_back(parseExpr());
            if (!check(TokenType::COMMA)) {
                break;
            }
            advance();
        }

        expect(TokenType::RPAREN, "Expected ')'");
        if (auto cnt = elems.size();
            0 == cnt) {
            throw std::runtime_error{"Unsupport the unit type."};
        } else if (1 == cnt) {
            // 单个表达式没有逗号不是元组，而是普通括号表达式
            return std::make_shared<ast::ParenthesisExpr>(std::move(elems[0]));
        } else {
            return std::make_shared<ast::TupleElements>(elems);
        }
    }

    ast::RefType ref_type {ast::RefType::Normal};
    if (check(TokenType::REF)) {
        advance();
        if (check(TokenType::MUT)) {
            advance();
            ref_type = ast::RefType::Mutable;
        } else {
            ref_type = ast::RefType::Immutable;
        }
    }

    auto element = parseElement(elem);

    return std::make_shared<ast::Factor>(ref_type, std::move(element));
}

/**
 * @brief  解析元素
 * @return ast::Number or ast::Variable or ast::ParenthesisExpr or
 *         ast::AssignElement or CallExpr
 */
[[nodiscard]]
ast::ExprPtr Parser::parseElement(std::optional<ast::AssignElementPtr> elem) {
    using TokenType = lexer::token::Type;

    if (elem.has_value()) {
        return elem.value();
    }

    if (check(TokenType::LPAREN)) {
        advance();
        ast::ExprPtr expr = Parser::parseCmpExpr();
        expect(TokenType::RPAREN, "Expected ')'");
        return std::make_shared<ast::ParenthesisExpr>(std::move(expr));
    }
    if (check(TokenType::INT)) {
        int value = std::stoi(current.getValue());
        advance();
        return std::make_shared<ast::Number>(value);
    } else if (check(TokenType::ID)) {
        if (checkAhead(TokenType::LBRACK) || checkAhead(TokenType::DOT)){
            auto value = parseAssignElement();
            return (std::move(value));
        } else if (checkAhead(TokenType::LPAREN)) {
            return parseCallExpr();
        } else {
            std::string name = current.getValue();
            advance();
            return std::make_shared<ast::Variable>(std::move(name));
        }
    } else if (check(TokenType::OP_MUL) && checkAhead(TokenType::ID)) {
        return parseAssignElement();
    } else {
        throw std::runtime_error("Unexpected token in expression: " + current.getValue());
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
    std::string name = current.getValue(); // function name
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
    auto if_branch  = parseBlockStmt();

    std::vector<ast::ElseClausePtr> else_clauses{};
    while(check(TokenType::ELSE)) {
        advance();
        bool end = !check(TokenType::IF);
        else_clauses.push_back(parseElseClause());
        if (end) {
            break;
        }
    }

    return std::make_shared<ast::IfStmt>(std::move(expr),
        std::move(if_branch), std::move(else_clauses));
}

/**
 * @brief  解析 else/else if 语句
 * @return ast::ElseClausePtr - AST Else Statement 结点指针
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

    bool mut = false;
    if (check(TokenType::MUT)) {
        mut = true;
        advance();
    }
    expect(TokenType::ID, "Expected '<ID>'");
    ast::VarDeclBodyPtr var = std::make_shared<ast::VarDeclBody>(mut, current.getValue());

    expect(TokenType::IN,"Expected 'in'");

    auto expr1 = parseCmpExpr();
    expect(TokenType::DOTS,"Expected '..'");

    auto expr2 = parseCmpExpr();
    auto block = parseBlockStmt();

    return std::make_shared<ast::ForStmt>(std::move(var),
        std::move(expr1), std::move(expr2), std::move(block));
}

/**
 * @brief  解析 loop 语句
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
    if (check(TokenType::REF)) {
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
        ast::VarTypePtr elem_type = parseVarType();
        expect(TokenType::SEMICOLON,"Expected ';' for Array");
        int cnt =std::stoi(current.getValue());
        expect(TokenType::INT,"Expected <NUM> for Array");
        expect(TokenType::RBRACK,"Expected ']' for Array");
        return std::make_shared<ast::Array>(cnt,elem_type,ref_type);
    }

    if (check(TokenType::LPAREN)) {
        advance();

        std::vector<ast::VarTypePtr> elem_types;
        while (!check(TokenType::RPAREN)) {
            elem_types.push_back(parseVarType());
            if (!check(TokenType::COMMA)) {
                break;
            }
            advance();
        }

        expect(TokenType::RPAREN, "Expected ')'");
        if (auto cnt = elem_types.size();
            0 == cnt) {
            throw std::runtime_error{"Incorrect variable type."};
        } else if (1 == cnt) {
            return elem_types[0];
        } else {
            return std::make_shared<ast::Tuple>(std::move(elem_types), ref_type);
        }
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
