#include <cassert>
#include <sstream>
#include <unordered_map>
#include "include/ast.hpp"

namespace parser::ast {

static int cnt = 0;

struct DotNodeDecl {
    std::string name  {};
    std::string label {};

    DotNodeDecl() = default;
    explicit DotNodeDecl(const std::string& n, const std::string& l)
        : name(n), label(l) {}
    explicit DotNodeDecl(std::string&& n, std::string&& l)
        : name(std::move(n)), label(std::move(l)) {}

    ~DotNodeDecl() = default;

    inline bool initialized() const {
        return name.length() > 0 && label.length() > 0;
    }
    inline std::string toString() const { return name + label; }
};

static DotNodeDecl str2NodeDecl(const std::string& s) {
    std::ostringstream oss;
    oss << s << cnt++;
    std::string name {oss.str()};

    std::string label = "[label = \"" + s + "\"]";

    return DotNodeDecl{name, label};
}

static DotNodeDecl tokenType2NodeDecl(const lexer::token::Type& t) {
    using TokenType = lexer::token::Type;
    static const std::unordered_map<TokenType, std::string> maps {
        {TokenType::REF,       "&"},
        {TokenType::LPAREN,    "("},
        {TokenType::RPAREN,    ")"},
        {TokenType::LBRACE,    "{"},
        {TokenType::RBRACE,    "}"},
        {TokenType::LBRACK,    "["},
        {TokenType::RBRACK,    "]"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::COLON,     ":"},
        {TokenType::COMMA,     ","},
        {TokenType::OP_PLUS,   "+"},
        {TokenType::ASSIGN,    "="},
        {TokenType::OP_MINUS,  "-"},
        {TokenType::OP_MUL,    "*"},
        {TokenType::OP_DIV,    "/"},
        {TokenType::OP_GT,     ">"},
        {TokenType::OP_LT,     "<"},
        {TokenType::DOT,       "."},
        {TokenType::OP_EQ,     "=="},
        {TokenType::OP_NEQ,    "!="},
        {TokenType::OP_GE,     ">="},
        {TokenType::OP_LE,     "<="},
        {TokenType::DOTS,      ".."},
        {TokenType::ARROW,     "->"},
        {TokenType::IF,        "if"},
        {TokenType::ELSE,      "else"},
        {TokenType::WHILE,      "while"}
    };

    if (maps.find(t) == maps.end()) {
        throw std::runtime_error{"tokenType2NodeDecl(): Unknown Token Type."};
    }

    std::ostringstream oss;
    oss << lexer::token::tokenType2str(t) << cnt++;
    std::string name {oss.str()};

    std::string label = "[label = \"" + maps.find(t)->second + "\"]";

    return DotNodeDecl{name, label};
}

template<typename... T> // 变长参数模板
static std::string nodeDecls2Str(const T&... nd) {
    static_assert((std::is_same_v<T, DotNodeDecl> && ...),
        "All arguments must be DotNodeDecl"); // 编译期检查，未通过则编译出错

    std::ostringstream oss;
    ((oss << "    " << nd.toString() << std::endl), ...); // 左折叠展开

    return oss.str();
}

inline static std::string edge2Str(const DotNodeDecl& a, const DotNodeDecl& b) {
    std::ostringstream oss;
    oss << "    " << a.name << " -> " << b.name << std::endl;
    return oss.str();
}

static std::string edges2Str(std::initializer_list<std::pair<DotNodeDecl, DotNodeDecl>> edges) {
    std::ostringstream oss;

    for(auto edge : edges) {
        oss << "    " << edge.first.name << " -> " << edge.second.name << std::endl;
    }
    return oss.str();
}

static auto varDeclBody2Dot(const VarDeclBodyPtr& vdb) {
    DotNodeDecl n_vdb     = str2NodeDecl("VarDeclBody");
    DotNodeDecl n_id      = str2NodeDecl("ID");
    DotNodeDecl n_id_name = str2NodeDecl(vdb->name);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;
    if (true == vdb->mut) {
        DotNodeDecl n_mut = str2NodeDecl("mut");
        oss_nd << nodeDecls2Str(n_vdb, n_mut, n_id, n_id_name);
        oss_ed << edge2Str(n_vdb, n_mut);
    } else {
        oss_nd << nodeDecls2Str(n_vdb, n_id, n_id_name);
    }

    oss_ed << edges2Str({{n_vdb, n_id}, {n_id, n_id_name}});

    return std::make_tuple(n_vdb, oss_nd.str(), oss_ed.str());
}

static auto integer2Dot(const IntegerPtr& integer) {
    DotNodeDecl n_int = str2NodeDecl("Integer");
    DotNodeDecl n_i32 = str2NodeDecl("i32");

    DotNodeDecl n_ref {},
                n_mut {};
    switch(integer->ref_type) {
    default:
        throw std::runtime_error{"Incorrect Reference Type."};
    case RefType::Normal:
        break;
    case RefType::Immutable:
        n_ref = tokenType2NodeDecl(lexer::token::Type::REF);
        break;
    case RefType::Mutable:
        n_ref = tokenType2NodeDecl(lexer::token::Type::REF);
        n_mut = str2NodeDecl("mut");
        break;
    }

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;

    oss_nd << nodeDecls2Str(n_int);
    if (n_ref.initialized()) {
        oss_nd << nodeDecls2Str(n_ref);
        if (n_mut.initialized()) {
            oss_nd << nodeDecls2Str( n_mut, n_i32);
            oss_ed << edge2Str(n_int, n_mut);
        }
        oss_ed << edge2Str(n_int, n_ref);
    }
    oss_nd << nodeDecls2Str(n_i32);
    oss_ed << edge2Str(n_int, n_i32);

    return std::make_tuple(n_int, oss_nd.str(), oss_ed.str());
}

// static std::tuple<DotNodeDecl, std::string, std::string> array2Dot(const ArrayPtr& arr) {

// }

// static std::tuple<DotNodeDecl, std::string, std::string> tuple2Dot(const TuplePtr& tup) {

// }

/**
 * @brief   ast::VarType 转 dot 格式
 * @param   vt AST Variable Type 结点指针
 * @return  [根节点的 DotNodeDecl, 结点声明串, 边声明串]
 */
static auto varType2Dot(const VarTypePtr& vt) {
    auto n_vt = str2NodeDecl("VarType");

    DotNodeDecl rt {};
    std::string nd {};
    std::string ed {};
    switch (vt->type()) {
    default:
        throw std::runtime_error{"varType2Dot(): Incorrect NodeType"};
        break;
    case NodeType::Integer:
        std::tie(rt, nd, ed) = integer2Dot(std::dynamic_pointer_cast<Integer>(vt));
        break;
    case NodeType::Array:
        // std::tie(rt, nd, ed) = array2Dot(std::dynamic_pointer_cast<Array>(vt));
        break;
    case NodeType::Tuple:
        // std::tie(rt, nd, ed) = tuple2Dot(std::dynamic_pointer_cast<Tuple>(vt));
        break;
    }

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;
    oss_nd << nodeDecls2Str(n_vt) << nd;
    oss_ed << edge2Str(n_vt, rt) << ed;

    return std::make_tuple(n_vt, oss_nd.str(), oss_ed.str());
}

static auto arg2Dot(const ArgPtr& arg) {
    DotNodeDecl n_arg = str2NodeDecl("Arg");

    auto [n_vdb, nd_vdb, ed_vdb] = varDeclBody2Dot(arg->variable);
    auto [n_vt, nd_vt, ed_vt]    = varType2Dot(arg->var_type);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;
    oss_nd << nodeDecls2Str(n_arg, n_vdb, n_vt) << nd_vdb << nd_vt;
    oss_ed << edge2Str(n_arg, n_vdb) << edge2Str(n_arg, n_vt) << ed_vdb << ed_vt;

    return std::make_tuple(n_arg, oss_nd.str(), oss_ed.str());
}
static auto assignElement2Dot(const AssignElementPtr& ae) {
    DotNodeDecl n_assign_elem = str2NodeDecl("AssignElement");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_assign_elem);

    switch (ae->kind) {
    case AssignElement::Kind::Variable: {
        auto var = std::dynamic_pointer_cast<ast::Variable>(ae);
        DotNodeDecl n_var = str2NodeDecl(var->name);
        oss_nd << nodeDecls2Str(n_var);
        oss_ed << edge2Str(n_assign_elem, n_var);
        break;
    }
    default:
        break;
    }

    return std::make_tuple(n_assign_elem, oss_nd.str(), oss_ed.str());
}

static auto funcHeaderDecl2Dot(const FuncHeaderDeclPtr& fhd) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_fhd     = str2NodeDecl("FuncHeaderDecl");
    DotNodeDecl n_fn      = str2NodeDecl("fn");
    DotNodeDecl n_id      = str2NodeDecl("ID");
    DotNodeDecl n_id_name = str2NodeDecl(fhd->name);

    DotNodeDecl n_lparen  = tokenType2NodeDecl(TokenType::LPAREN);
    DotNodeDecl n_rparen  = tokenType2NodeDecl(TokenType::RPAREN);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;
    oss_nd << nodeDecls2Str(n_fhd, n_fn, n_id, n_id_name, n_lparen);
    oss_ed << edges2Str({{n_fhd, n_fn}, {n_fhd, n_id}, {n_id, n_id_name}, {n_fhd, n_lparen}});

    for (auto it = fhd->argv.begin(); it != fhd->argv.end(); ++it) {
        auto [rt, nd, ed] = arg2Dot(*it);
        oss_nd << nd;
        oss_ed << ed << edge2Str(n_fhd, rt);
        if (std::next(it) != fhd->argv.end()) {
            DotNodeDecl n_comma = tokenType2NodeDecl(TokenType::COMMA);
            oss_nd << nodeDecls2Str(n_comma);
            oss_ed << edge2Str(n_fhd, n_comma);
        }
    }
    oss_nd << nodeDecls2Str(n_rparen);
    oss_ed << edge2Str(n_fhd, n_rparen);
    if (fhd->retval_type.has_value()){
        DotNodeDecl n_arrow = tokenType2NodeDecl(TokenType::ARROW);
        oss_nd << nodeDecls2Str(n_arrow);
        oss_ed << edge2Str(n_fhd, n_arrow);
        auto [n_vt, nd_vt, ed_vt] = varType2Dot(fhd->retval_type.value());
        oss_nd << nd_vt;
        oss_ed << edge2Str(n_fhd, n_vt) << ed_vt;
    }



    return std::make_tuple(n_fhd, oss_nd.str(), oss_ed.str());
}

static auto numberExpr2Dot(const std::shared_ptr<ast::Number>& n) {
    DotNodeDecl n_num = str2NodeDecl("Number");
    DotNodeDecl n_val = str2NodeDecl(std::to_string(n->value));

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_num, n_val);
    oss_ed << edge2Str(n_num, n_val);

    return std::make_tuple(n_num, oss_nd.str(), oss_ed.str());
}

static auto variableExpr2Dot(const std::shared_ptr<ast::Variable>& v) {
    DotNodeDecl v_id = str2NodeDecl("ID");
    DotNodeDecl v_name = str2NodeDecl(v->name);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(v_id, v_name);
    oss_ed << edge2Str(v_id, v_name);

    return std::make_tuple(v_id, oss_nd.str(), oss_ed.str());
}

static std::tuple<DotNodeDecl, std::string, std::string> expr2Dot(const ExprPtr &expr);

static auto factorExpr2Dot(const FactorPtr& f) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_factor = str2NodeDecl("Factor");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_factor);

    //DEBUG 打印错误
    if (f->ref_type != RefType::Normal) {
        std::string ref_str;
        switch (f->ref_type) {
        case RefType::Immutable:
            ref_str = tokenType2str(TokenType::REF);
            break;
        case RefType::Mutable:
            ref_str = "&mut";
            break;
        default:
            ref_str = "?";
            break;
        }

        DotNodeDecl n_ref = str2NodeDecl(ref_str);
        oss_nd << nodeDecls2Str(n_ref);
        oss_ed << edge2Str(n_factor, n_ref);
    }

    auto [n_inner, inner_nd, inner_ed] = expr2Dot(f->element);
    oss_nd << inner_nd;
    oss_ed << inner_ed << edge2Str(n_factor, n_inner);

    return std::make_tuple(n_factor, oss_nd.str(), oss_ed.str());
}

static auto arithmeticExpr2Dot(const std::shared_ptr<ast::ArithmeticExpr>& ae) {
    DotNodeDecl n_arith = str2NodeDecl("ArithmeticExpr");


    auto [n_lhs, lhs_nd, lhs_ed] = expr2Dot(ae->lhs);
    DotNodeDecl n_op = tokenType2NodeDecl(ae->op);
    auto [n_rhs, rhs_nd, rhs_ed] = expr2Dot(ae->rhs);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << lhs_nd << nodeDecls2Str(n_arith, n_op) << rhs_nd;
    oss_ed << lhs_ed
        << edges2Str({{n_arith, n_lhs}, {n_arith, n_op}, {n_arith, n_rhs}})
        << rhs_ed;

    return std::make_tuple(n_arith, oss_nd.str(), oss_ed.str());
}

static auto callExpr2Dot(const std::shared_ptr<ast::CallExpr>& ce) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_call   = str2NodeDecl("CallExpr");
    DotNodeDecl n_id     = str2NodeDecl("ID");
    DotNodeDecl n_fn     = str2NodeDecl(ce->callee);

    DotNodeDecl n_lparen = tokenType2NodeDecl(TokenType::LPAREN);
    DotNodeDecl n_rparen = tokenType2NodeDecl(TokenType::RPAREN);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_call, n_id, n_fn, n_lparen);
    oss_ed << edges2Str({{n_call, n_id},{n_id, n_fn}, {n_call, n_lparen}});

    if (!ce->argv.empty()) {
        DotNodeDecl n_arglist = str2NodeDecl("ArgList");
        oss_nd << nodeDecls2Str(n_arglist, n_rparen);
        oss_ed << edges2Str({{n_call, n_arglist}, {n_call, n_rparen}});

        for (const auto& arg : ce->argv) {
            auto [n_arg, arg_nd, arg_ed] = expr2Dot(arg);
            oss_nd << arg_nd;
            oss_ed << arg_ed << edge2Str(n_arglist, n_arg);
        }
    } else {
        oss_nd << nodeDecls2Str(n_rparen);
        oss_ed << edge2Str(n_call, n_rparen);
    }

    return std::make_tuple(n_call, oss_nd.str(), oss_ed.str());
}

static auto parenthesisExpr2Dot(const std::shared_ptr<ast::ParenthesisExpr>& pe) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_paren = str2NodeDecl("ParenthesisExpr");

    DotNodeDecl n_lparen = tokenType2NodeDecl(TokenType::LPAREN);
    DotNodeDecl n_rparen = tokenType2NodeDecl(TokenType::RPAREN);

    std::ostringstream oss_nd, oss_ed;

    auto [n_inner, inner_nd, inner_ed] = expr2Dot(pe->expr);
    oss_nd << nodeDecls2Str(n_lparen, n_paren) << inner_nd << nodeDecls2Str(n_paren, n_rparen);
    oss_ed << edges2Str({{n_paren, n_lparen}, {n_paren, n_inner}, {n_paren, n_rparen}}) << inner_ed;

    return std::make_tuple(n_paren, oss_nd.str(), oss_ed.str());
}

static auto element2Dot(const ExprPtr& e){
    using enum ast::NodeType;

    DotNodeDecl n_element = str2NodeDecl("Element");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_element);

    DotNodeDecl n_inner;
    std::string inner_nd, inner_ed;

    switch (e->type()) {
    case Number:
        std::tie(n_inner, inner_nd, inner_ed) =
            numberExpr2Dot(std::dynamic_pointer_cast<ast::Number>(e));
        break;
    case Variable:
        std::tie(n_inner, inner_nd, inner_ed) =
            variableExpr2Dot(std::dynamic_pointer_cast<ast::Variable>(e));
        break;
    case CallExpr:
        std::tie(n_inner, inner_nd, inner_ed) =
            callExpr2Dot(std::dynamic_pointer_cast<ast::CallExpr>(e));
        break;
    case ParenthesisExpr:
        std::tie(n_inner, inner_nd, inner_ed) =
            parenthesisExpr2Dot(std::dynamic_pointer_cast<ast::ParenthesisExpr>(e));
        break;
    default:
        n_inner = str2NodeDecl("UnknownElement");
        inner_nd = nodeDecls2Str(n_inner);
        break;
    }

    oss_nd << inner_nd;
    oss_ed << inner_ed << edge2Str(n_element, n_inner);

    return std::make_tuple(n_element, oss_nd.str(), oss_ed.str());
}
static std::tuple<DotNodeDecl, std::string, std::string> expr2Dot(const ExprPtr& expr) {
    using enum ast::NodeType;

    DotNodeDecl rt {};
    std::string nd {};
    std::string ed {};

    switch (expr->type()) {
    case Number:
    case Variable:
    case CallExpr:
    case ParenthesisExpr:
        std::tie(rt, nd, ed) = element2Dot(expr);
        break;
    case Factor:
        std::tie(rt, nd, ed) = factorExpr2Dot(std::dynamic_pointer_cast<ast::Factor>(expr));
        break;
    case ArithmeticExpr:
        std::tie(rt, nd, ed) = arithmeticExpr2Dot(std::dynamic_pointer_cast<ast::ArithmeticExpr>(expr));
        break;
    default:
        rt = str2NodeDecl("UnknownExpr");
        nd = nodeDecls2Str(rt);
        ed = "";
        break;
    }

    return std::make_tuple(rt, nd, ed);
}

static auto exprStmt2Dot(const std::shared_ptr<ast::ExprStmt>& es) {
    DotNodeDecl n_es = str2NodeDecl("ExprStmt");
    auto [n_expr, expr_nd, expr_ed] = expr2Dot(es->expr);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_es) << expr_nd;
    oss_ed << edge2Str(n_es, n_expr) << expr_ed;

    return std::make_tuple(n_es, oss_nd.str(), oss_ed.str());
}

static auto returnStmt2Dot(const std::shared_ptr<ast::RetStmt>& rs) {
    DotNodeDecl n_rs = str2NodeDecl("RetStmt");
    DotNodeDecl n_ret = str2NodeDecl("return");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_rs, n_ret);
    oss_ed << edge2Str(n_rs, n_ret);

    if (rs->ret_val) {
        auto [n_expr, expr_nd, expr_ed] = expr2Dot(rs->ret_val.value());
        oss_nd << expr_nd;
        oss_ed << expr_ed << edge2Str(n_rs, n_expr);
    }

    return std::make_tuple(n_rs, oss_nd.str(), oss_ed.str());
}
static auto varDeclStmt2Dot(const VarDeclStmtPtr& vds) {
    using TokenType = lexer::token::Type;

    DotNodeDecl n_vds = str2NodeDecl("VarDeclStmt");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_vds);

    auto [n_var, var_nd, var_ed] = varDeclBody2Dot(vds->variable);
    oss_nd << var_nd;
    oss_ed << var_ed << edge2Str(n_vds, n_var);

    if (vds->var_type.has_value()) {
        DotNodeDecl n_colon = tokenType2NodeDecl(TokenType::COLON);
        oss_nd << nodeDecls2Str(n_colon);
        oss_ed << edge2Str(n_vds, n_colon);

        auto [n_type, type_nd, type_ed] = varType2Dot(vds->var_type.value());
        oss_nd << type_nd;
        oss_ed << type_ed << edge2Str(n_vds, n_type);
    }

    return std::make_tuple(n_vds, oss_nd.str(), oss_ed.str());
}
static auto assignStmt2Dot(const AssignStmtPtr& as) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_as = str2NodeDecl("AssignStmt");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_as);

    auto [n_lv, lv_nd, lv_ed] = assignElement2Dot(as->lvalue);
    oss_nd << lv_nd;
    oss_ed << lv_ed << edge2Str(n_as, n_lv);

    DotNodeDecl n_assign = tokenType2NodeDecl(TokenType::ASSIGN);
    oss_nd << nodeDecls2Str(n_assign);
    oss_ed << edge2Str(n_as, n_assign);

    auto [n_expr, expr_nd, expr_ed] = expr2Dot(as->expr);
    oss_nd << expr_nd;
    oss_ed << expr_ed << edge2Str(n_as, n_expr);

    return std::make_tuple(n_as, oss_nd.str(), oss_ed.str());
}

static auto varDeclAssignStmt2Dot(const VarDeclAssignStmtPtr& vdas) {
    using TokenType = lexer::token::Type;
    DotNodeDecl n_vdas = str2NodeDecl("VarDeclAssignStmt");

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_vdas);

    auto [n_var, var_nd, var_ed] = varDeclBody2Dot(vdas->variable);
    oss_nd << var_nd;
    oss_ed << var_ed << edge2Str(n_vdas, n_var);

    if (vdas->var_type.has_value()) {
        DotNodeDecl n_colon = tokenType2NodeDecl(TokenType::COLON);
        oss_nd << nodeDecls2Str(n_colon);
        oss_ed << edge2Str(n_vdas, n_colon);
        auto [n_type, type_nd, type_ed] = varType2Dot(vdas->var_type.value());
        oss_nd << type_nd;
        oss_ed << type_ed << edge2Str(n_vdas, n_type);
    }

    DotNodeDecl n_assign = tokenType2NodeDecl(TokenType::ASSIGN);
    oss_nd << nodeDecls2Str(n_assign);
    oss_ed << edge2Str(n_vdas, n_assign);

    auto [n_expr, expr_nd, expr_ed] = expr2Dot(vdas->expr);
    oss_nd << expr_nd;
    oss_ed << expr_ed << edge2Str(n_vdas, n_expr);

    return std::make_tuple(n_vdas, oss_nd.str(), oss_ed.str());
}
static std::tuple<DotNodeDecl, std::string, std::string> stmt2Dot(const StmtPtr &stmt);
static std::tuple<DotNodeDecl, std::string, std::string> blockStmt2Dot(const BlockStmtPtr &bs)
{
    using TokenType = lexer::token::Type;

    DotNodeDecl n_bs = str2NodeDecl("BlockStmt");
    DotNodeDecl n_lbrace = tokenType2NodeDecl(TokenType::LBRACE);
    DotNodeDecl n_rbrace = tokenType2NodeDecl(TokenType::RBRACE);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;

    oss_nd << nodeDecls2Str(n_bs);
    oss_nd << nodeDecls2Str(n_lbrace);
    oss_ed << edge2Str(n_bs, n_lbrace);

    for (const auto &stmt : bs->stmts) {
        auto [n_stmt, stmt_nd, stmt_ed] = stmt2Dot(stmt);
        oss_nd << stmt_nd;
        oss_ed << stmt_ed << edge2Str(n_bs, n_stmt);
    }

    oss_nd << nodeDecls2Str(n_rbrace);
    oss_ed << edge2Str(n_bs, n_rbrace);

    return std::make_tuple(n_bs, oss_nd.str(), oss_ed.str());
}
static auto ifStmt2Dot(const IfStmtPtr& istmt) {
    using TokenType = lexer::token::Type;

    DotNodeDecl n_if_stmt = str2NodeDecl("IfStmt");
    DotNodeDecl n_if_token = tokenType2NodeDecl(TokenType::IF);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_if_stmt, n_if_token);
    oss_ed << edge2Str(n_if_stmt, n_if_token);

    auto [n_cond, cond_nd, cond_ed] = expr2Dot(istmt->expr);
    oss_nd << cond_nd;
    oss_ed << cond_ed << edge2Str(n_if_stmt, n_cond);

    auto [n_if_blk, blk_nd, blk_ed] = blockStmt2Dot(istmt->if_branch);
    oss_nd << blk_nd;
    oss_ed << blk_ed << edge2Str(n_if_stmt, n_if_blk);

    for (const auto& clause : istmt->else_clauses) {
        if (clause->expr.has_value()) {
            DotNodeDecl n_else_if = str2NodeDecl("else_if");
            oss_nd << nodeDecls2Str(n_else_if);
            oss_ed << edge2Str(n_if_stmt, n_else_if);  
    
            auto [n_expr, e_nd, e_ed] = expr2Dot(clause->expr.value());
            oss_nd << e_nd;
            oss_ed << e_ed << edge2Str(n_if_stmt, n_expr);

            auto [n_blk, blk_nd, blk_ed] = blockStmt2Dot(clause->block);
            oss_nd << blk_nd;
            oss_ed << blk_ed << edge2Str(n_if_stmt, n_blk);
        } else {
            // 纯 else
            DotNodeDecl n_else = tokenType2NodeDecl(TokenType::ELSE);
            oss_nd << nodeDecls2Str(n_else);
            oss_ed << edge2Str(n_if_stmt, n_else);
    
            auto [n_blk, blk_nd, blk_ed] = blockStmt2Dot(clause->block);
            oss_nd << blk_nd;
            oss_ed << blk_ed << edge2Str(n_if_stmt, n_blk);
        }
    }
    

    return std::make_tuple(n_if_stmt, oss_nd.str(), oss_ed.str());
}
static auto whileStmt2Dot(const WhileStmtPtr& ws) {
    using TokenType = lexer::token::Type;

    DotNodeDecl n_while_stmt = str2NodeDecl("WhileStmt");
    DotNodeDecl n_while_kw   = tokenType2NodeDecl(TokenType::WHILE);

    std::ostringstream oss_nd, oss_ed;
    oss_nd << nodeDecls2Str(n_while_stmt, n_while_kw);
    oss_ed << edge2Str(n_while_stmt, n_while_kw);

    auto [n_expr, expr_nd, expr_ed] = expr2Dot(ws->expr);
    oss_nd << expr_nd;
    oss_ed << expr_ed << edge2Str(n_while_stmt, n_expr);


    auto [n_block, block_nd, block_ed] = blockStmt2Dot(ws->block);
    oss_nd << block_nd;
    oss_ed << block_ed << edge2Str(n_while_stmt, n_block);

    return std::make_tuple(n_while_stmt, oss_nd.str(), oss_ed.str());
}


static std::tuple<DotNodeDecl, std::string, std::string> stmt2Dot(const StmtPtr &stmt) {
    using enum ast::NodeType;
    using TokenType = lexer::token::Type;

    DotNodeDecl rt{};
    std::string nd{};
    std::string ed{};

    switch (stmt->type()) {
    case ExprStmt:
        std::tie(rt, nd, ed) = exprStmt2Dot(std::dynamic_pointer_cast<ast::ExprStmt>(stmt));
        break;
    case RetStmt:
        std::tie(rt, nd, ed) = returnStmt2Dot(std::dynamic_pointer_cast<ast::RetStmt>(stmt));
        break;
    case VarDeclStmt:
        std::tie(rt, nd, ed) = varDeclStmt2Dot(std::dynamic_pointer_cast<ast::VarDeclStmt>(stmt));
        break;
    case AssignStmt:
        std::tie(rt, nd, ed) = assignStmt2Dot(std::dynamic_pointer_cast<ast::AssignStmt>(stmt));
        break;
    case VarDeclAssignStmt:
        std::tie(rt, nd, ed) = varDeclAssignStmt2Dot(std::dynamic_pointer_cast<ast::VarDeclAssignStmt>(stmt));
        break;
    case IfStmt:
        std::tie(rt, nd, ed) = ifStmt2Dot(std::dynamic_pointer_cast<ast::IfStmt>(stmt));
        return std::make_tuple(rt, nd, ed);  // 不加分号
    case WhileStmt:
        std::tie(rt, nd, ed) = whileStmt2Dot(std::dynamic_pointer_cast<ast::WhileStmt>(stmt));
        return std::make_tuple(rt, nd, ed); // 不加分号
    default:
        rt = str2NodeDecl("UnknownStmt");
        nd = nodeDecls2Str(rt);
        ed = "";
        break;
    }

    // 为普通语句添加分号
    DotNodeDecl n_semi = tokenType2NodeDecl(TokenType::SEMICOLON);
    std::ostringstream oss_nd, oss_ed;
    oss_nd << nd << nodeDecls2Str(n_semi);
    oss_ed << ed << edge2Str(rt, n_semi);

    return std::make_tuple(rt, oss_nd.str(), oss_ed.str());
}




static auto funcDecl2Dot(const FuncDeclPtr& fd) {
    DotNodeDecl n_fd = str2NodeDecl("FuncDecl");

    auto [n_fhd, fhd_nd, fhd_ed] = funcHeaderDecl2Dot(fd->header);
    auto [n_bs, bs_nd, bs_ed] = blockStmt2Dot(fd->body);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;

    oss_nd << nodeDecls2Str(n_fd) << fhd_nd << bs_nd;
    oss_ed << edges2Str({{n_fd, n_fhd}, {n_fd, n_bs}}) << fhd_ed << bs_ed;

    return std::make_tuple(n_fd, oss_nd.str(), oss_ed.str());
}

void ast2Dot(std::ofstream& out, const ProgPtr& prog) {
    out << "digraph AST {" << std::endl
        << "    node [shape=ellipse, fontname=\"Courier\"]" << std::endl << std::endl
        << "    // define nodes" << std::endl;

    DotNodeDecl n_prog = str2NodeDecl("Prog");

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;

    oss_nd << nodeDecls2Str(n_prog);
    for (const auto& decl : prog->decls) {
        assert(std::dynamic_pointer_cast<FuncDecl>(decl));
        auto [n_fd, fd_nd, fd_ed] = funcDecl2Dot(std::dynamic_pointer_cast<FuncDecl>(decl));
        oss_nd << fd_nd;
        oss_ed << edge2Str(n_prog, n_fd) << fd_ed;
    }

    out << oss_nd.str() << std::endl
        << "    // define edges" << std::endl
        << oss_ed.str() << std::endl
        << "}" << std::endl;
}

} // namespace parser::ast
