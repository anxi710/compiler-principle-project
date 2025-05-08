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

    bool initialized() {
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
        {TokenType::ARROW,     "->"}
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

template<typename... T>
static std::string edges2Str(const T&... e) {
    static_assert((std::is_same_v<T, std::pair<DotNodeDecl, DotNodeDecl>> && ...),
        "All argument must be pair<const DotNodeDecl&, const DotNodeDecl&>");

    std::ostringstream oss;
    ((oss << "    " << e.first.name << " -> " << e.second.name << std::endl), ...);
    return oss.str();
}

static auto varDeclBody2Dot(const VarDeclBodyPtr& vdb) {
    DotNodeDecl n_vdb     = str2NodeDecl("VarDeclBody");
    DotNodeDecl n_id      = str2NodeDecl("ID");
    DotNodeDecl n_id_name = str2NodeDecl(vdb->name);

    std::string node_decl {};
    std::string edge_decl {};
    if (true == vdb->mut) {
        DotNodeDecl n_mut = str2NodeDecl("mut");
        node_decl  = nodeDecls2Str(n_vdb, n_mut, n_id, n_id_name);
        edge_decl += edge2Str(n_vdb, n_mut);
    } else {
        node_decl = nodeDecls2Str(n_vdb, n_id, n_id_name);
    }

    edge_decl += edges2Str(std::make_pair(n_vdb, n_id), std::make_pair(n_id, n_id_name));

    return std::make_tuple(n_vdb, node_decl, edge_decl);
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

static std::tuple<DotNodeDecl, std::string, std::string> array2Dot(const ArrayPtr& arr) {

}

static std::tuple<DotNodeDecl, std::string, std::string> tuple2Dot(const TuplePtr& tup) {

}

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

    std::string node_decl = nodeDecls2Str(n_vt) + nd;
    std::string edge_decl = edge2Str(n_vt, rt) + ed;

    return std::make_tuple(n_vt, node_decl, edge_decl);
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
    oss_ed << edges2Str(
        std::make_pair(n_fhd, n_fn),
        std::make_pair(n_fhd, n_id),
        std::make_pair(n_id, n_id_name),
        std::make_pair(n_fhd, n_lparen)
    );

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

    return std::make_tuple(n_fhd, oss_nd.str(), oss_ed.str());
}

static std::tuple<DotNodeDecl, std::string, std::string> blockStmt2Dot(const BlockStmtPtr& bs) {

}

static auto funcDecl2Dot(const FuncDeclPtr& fd) {
    DotNodeDecl n_fd = str2NodeDecl("FuncDecl");

    auto [n_fhd, fhd_nd, fhd_ed] = funcHeaderDecl2Dot(fd->header);
    // auto [n_bs, bs_nd, bs_ed] = blockStmt2Dot(fd->body);

    std::ostringstream oss_nd;
    std::ostringstream oss_ed;

    oss_nd << nodeDecls2Str(n_fd) << fhd_nd;
    oss_ed << edge2Str(n_fd, n_fhd) << fhd_ed; //TODO 补充 BlockStmt

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
