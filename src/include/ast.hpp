#pragma once

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <optional>
#include "token.hpp"

namespace parser::ast {

// 如果基类析构函数没有被声明为虚函数，则 C++ 只会调用基类的析构函数，
// 而不会调用派生类的析构函数

enum class NodeType {
    Prog, Arg,

    Decl, Stmt, Expr, VarType,

    VarDecl, FuncDecl, FuncHeaderDecl,

    BlockStmt, ExprStmt, RetStmt, VarDeclStmt, AssignStmt, VarDeclAssignStmt,
    ElseClause, IfStmt, WhileStmt, ForStmt, LoopStmt, BreakStmt, ContinueStmt, NullStmt,

    Variable, Number, Factor, ArithmeticExpr, CallExpr, FuncExprBlockStmt,

    Integer, Array, Tuple
};

// AST 结点
struct Node {
    virtual constexpr NodeType type() const = 0;
    virtual ~Node() = default;
};
using NodePtr = std::shared_ptr<Node>;

// Declaration
struct Decl : Node {
    constexpr NodeType type() const override {
        return NodeType::Decl;
    }

    virtual ~Decl() = default;
};
using  DeclPtr = std::shared_ptr<Decl>;

// Prog -> Decls
// Program
struct Prog : Node {
    std::vector<DeclPtr> decls; // declarations
                                // 隐含了 Decls -> Decl Decls | epsilon

    template<typename T>
    Prog(T&& decls) : decls(std::forward<T>(decls)) {}

    constexpr NodeType type() const override {
        return NodeType::Prog;
    }
};
using  ProgPtr = std::shared_ptr<Prog>;

struct VarDecl : Decl {
    bool        mutable_; // mutable or not
    std::string name;     // variable name

    VarDecl() = default;
    VarDecl(bool mutable_, std::string name) : mutable_(mutable_), name(name) {}

    constexpr NodeType type() const override {
        return NodeType::VarDecl;
    }
};
using  VarDeclPtr = std::shared_ptr<VarDecl>;

enum class RefType {
    normal,    // 普通变量
    deref,     // 解引用
    immutable, // 不可变引用
    mutableref // 可变引用
};

struct VarType : Node {
    RefType ref_type;

    VarType() : ref_type(RefType::normal) {}
    VarType(RefType ref_type) : ref_type(ref_type) {}
    virtual ~VarType() = default;

    constexpr NodeType type() const override {
        return NodeType::VarType;
    }
};
using  VarTypePtr = std::shared_ptr<VarType>;

struct Integer : VarType {
    Integer() : VarType() {}
    Integer(RefType ref_type) : VarType(ref_type) {}

    constexpr NodeType type() const override {
        return NodeType::Integer;
    }
};
using  IntegerPtr = std::shared_ptr<Integer>;

struct Array : VarType {
    int cnt; // 数组中元素个数

    Array() : VarType(), cnt(0) {}
    Array(int cnt, RefType ref_type = RefType::normal) : VarType(ref_type), cnt(cnt) {}

    constexpr NodeType type() const override {
        return NodeType::Array;
    }
};
using  ArrayPtr = std::shared_ptr<Array>;

struct Tuple : VarType {
    int cnt; // 元组中元素个数

    Tuple() : VarType(), cnt(0) {}
    Tuple(int cnt, RefType ref_type = RefType::normal) : VarType(ref_type), cnt(cnt) {}

    constexpr NodeType type() const override {
        return NodeType::Tuple;
    }
};
using TuplePtr = std::shared_ptr<Tuple>;

// Argument
struct Arg : Node {
    VarDeclPtr variable;  // variable
    VarTypePtr var_type; // variable type

    template<typename T1, typename T2>
    Arg(T1&& var, T2&& type) : variable(std::forward<T1>(var)), var_type(std::forward<T2>(type)) {}

    constexpr NodeType type() const override {
        return NodeType::Arg;
    }
};
using  ArgPtr = std::shared_ptr<Arg>;

// Statement
struct Stmt : Node {
    virtual ~Stmt() = default;

    constexpr NodeType type() const override {
        return NodeType::Stmt;
    }
};
using  StmtPtr = std::shared_ptr<Stmt>;

// Block statement
struct BlockStmt : Stmt {
    std::vector<StmtPtr> stmts; // statements

    template<typename T>
    BlockStmt(T&& stmts) : stmts(std::forward<T>(stmts)) {}

    constexpr NodeType type() const override {
        return NodeType::BlockStmt;
    }
};
using  BlockStmtPtr = std::shared_ptr<BlockStmt>;

// FuncHeaderDecl -> fn <ID> ( Args )
// Function header declaration
struct FuncHeaderDecl : Decl {
    std::string               name;        // function name
    std::vector<ArgPtr>       argv;        // argument vector
    std::optional<VarTypePtr> retval_type; // return value type

    template<typename T1, typename T2>
    FuncHeaderDecl(std::string name, T1&& argv, T2&& retval_type)
        : name(name), argv(std::forward<T1>(argv)),
          retval_type(std::forward<T2>(retval_type)) {}

    constexpr NodeType type() const override {
        return NodeType::FuncHeaderDecl;
    }
};
using  FuncHeaderDeclPtr = std::shared_ptr<FuncHeaderDecl>;

// Decl -> FuncDecl
// FuncDecl -> FuncHeaderDecl BlockStat
struct FuncDecl : Decl {
    FuncHeaderDeclPtr header; // function header
    BlockStmtPtr      body;   // function body

    template<typename T1, typename T2>
    FuncDecl(T1&& header, T2&& body)
        : header(std::forward<T1>(header)), body(std::forward<T2>(body)) {}

    constexpr NodeType type() const override {
        return NodeType::FuncDecl;
    }
};
using  FuncDeclPtr = std::shared_ptr<FuncDecl>;

// Expression
struct Expr : Node {
    virtual ~Expr() = default;

    constexpr NodeType type() const override {
        return NodeType::Expr;
    }
};
using  ExprPtr = std::shared_ptr<Expr>;

struct ExprStmt : Stmt {
    ExprPtr expr;

    template<typename T>
    ExprStmt(T&& expr) : expr(std::forward<T>(expr)) {}

    constexpr NodeType type() const override {
        return NodeType::ExprStmt;
    }
};
using  ExprStmtPtr = std::shared_ptr<ExprStmt>;

struct Variable : Expr {
    std::string name; // 变量名

    Variable() = default;
    template<typename T>
    Variable(T&& name) : name(std::forward<T>(name)) {}

    constexpr NodeType type() const override {
        return NodeType::Variable;
    }
};
using  VariablePtr = std::shared_ptr<Variable>;

struct Number : Expr {
    int value; // 值

    Number() = default;
    Number(int value) : value(value) {}

    constexpr NodeType type() const override {
        return NodeType::Number;
    }
};
using  NumberPtr = std::shared_ptr<Number>;

struct Factor : Expr {
    RefType ref_type;
    ExprPtr element;

    template<typename T>
    Factor(RefType ref_type, T&& element) : ref_type(ref_type), element(std::forward<T>(element)) {}

    constexpr NodeType type() const override {
        return NodeType::Factor;
    }
};
using  FactorPtr = std::shared_ptr<Factor>;

// Return Statement
struct RetStmt : Stmt {
    std::optional<ExprPtr> ret_val; // return value (an expression)

    template<typename T>
    RetStmt(T&& ret_val) : ret_val(std::forward<T>(ret_val)) {}

    constexpr NodeType type() const override {
        return NodeType::RetStmt;
    }
};
using  RetStmtPtr = std::shared_ptr<RetStmt>;

struct VarDeclStmt : Stmt, Decl {
    VarDeclPtr                variable; // variable
    std::optional<VarTypePtr> var_type; // variable type

    template<typename T1, typename T2>
    VarDeclStmt(T1&& var, T2&& type)
        : variable(std::forward<T1>(var)), var_type(std::forward<T2>(type)) {}

    constexpr NodeType type() const override {
        return NodeType::VarDeclStmt;
    }
};
using  VarDeclStmtPtr = std::shared_ptr<VarDeclStmt>;

struct AssignStmt : Stmt {
    std::string variable; // variable
    RefType     var_type; // dereference or normal
    ExprPtr     expr;     // expression

    template<typename T>
    AssignStmt(std::string var, RefType type, T&& expr)
        : variable(var), var_type(type), expr(std::forward<T>(expr)) {}

    constexpr NodeType type() const override {
        return NodeType::AssignStmt;
    }
};
using  AssignStmtPtr = std::shared_ptr<AssignStmt>;

struct VarDeclAssignStmt : VarDeclStmt {
    ExprPtr expr;

    template<typename T1, typename T2, typename T3>
    VarDeclAssignStmt(T1&& var, T2&& type, T3&& expr)
        : VarDeclStmt(std::forward<T1>(var), std::forward<T2>(type)), expr(std::forward<T3>(expr)) {}

    constexpr NodeType type() const override {
        return NodeType::VarDeclAssignStmt;
    }
};
using  VarDeclAssignStmtPtr = std::shared_ptr<VarDeclAssignStmt>;

struct ArithmeticExpr : Expr {
    ExprPtr            lhs; // 左操作数
    lexer::token::Type op;  // operator
    ExprPtr            rhs; // 右操作数

    ArithmeticExpr(ExprPtr lhs, lexer::token::Type op, ExprPtr rhs)
        : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

    constexpr NodeType type() const override {
        return NodeType::ArithmeticExpr;
    }
};
using  ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

struct CallExpr : Expr {
    std::string          callee;
    std::vector<ExprPtr> arguments;

    template<typename T>
    CallExpr(std::string name, T&& args) : callee(name), arguments(std::forward<T>(args)) {}

    constexpr NodeType type() const override {
        return NodeType::CallExpr;
    }
};
using  CallExprPtr = std::shared_ptr<CallExpr>;

struct ElseClause : Stmt {
    std::optional<ExprPtr>  expr;  // else (if expr)?
    BlockStmtPtr            block;

    template<typename T1, typename T2>
    ElseClause(T1&& expr, T2&& block)
        : expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}

    constexpr NodeType type() const override {
        return NodeType::ElseClause;
    }
};
using  ElseClausePtr = std::shared_ptr<ElseClause>;

struct IfStmt : Stmt {
    ExprPtr                     expr;
    BlockStmtPtr                block;
    std::vector<ElseClausePtr>  elcls; // else clauses

    template<typename T1, typename T2, typename T3>
    IfStmt(T1&& expr, T2&& block, T3&& clauses) : expr(std::forward<T1>(expr)),
        block(std::forward<T2>(block)), elcls(std::forward<T3>(clauses)) {}

    constexpr NodeType type() const override {
        return NodeType::IfStmt;
    }
};
using  IfStmtPtr = std::shared_ptr<IfStmt>;

struct WhileStmt : Stmt {
    ExprPtr         expr;
    BlockStmtPtr    block;

    template<typename T1, typename T2>
    WhileStmt(T1&& expr, T2&& block)
        : expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}

    constexpr NodeType type() const override {
        return NodeType::WhileStmt;
    }
};
using  WhileStmtPtr = std::shared_ptr<WhileStmt>;

struct ForStmt : Stmt {
    VarDeclPtr   var;
    ExprPtr      lexpr;
    ExprPtr      rexpr;
    BlockStmtPtr block;

    template<typename T, typename T1, typename T2, typename T3>
    ForStmt(T&& var, T1&& expr1, T2&& expr2, T3&& block) :
        var(std::forward<T>(var)), lexpr(std::forward<T1>(expr1)),
        rexpr(std::forward<T2>(expr2)), block(std::forward<T3>(block)){}

    constexpr NodeType type() const override {
        return NodeType::ForStmt;
    }
};
using  ForStmtPtr = std::shared_ptr<ForStmt>;

struct LoopStmt : Stmt {
    BlockStmtPtr    block;

    template<typename T>
    LoopStmt(T&& block) : block(std::forward<T>(block)){}

    constexpr NodeType type() const override {
        return NodeType::LoopStmt;
    }
};
using  LoopStmtPtr = std::shared_ptr<LoopStmt>;

struct BreakStmt : Stmt {
    ~BreakStmt() override = default;

    constexpr NodeType type() const override {
        return NodeType::BreakStmt;
    }
};
using  BreakStmtPtr = std::shared_ptr<BreakStmt>;

struct ContinueStmt : Stmt {
    ~ContinueStmt() override = default;

    constexpr NodeType type() const override {
        return NodeType::ContinueStmt;
    }
};
using  ContinueStmtPtr = std::shared_ptr<ContinueStmt>;

struct NullStmt : Stmt {
    ~NullStmt() override = default;

    constexpr NodeType type() const override {
        return NodeType::NullStmt;
    }
};
using  NullStmtPtr = std::shared_ptr<NullStmt>;

struct FuncExprBlockStmt : Expr, BlockStmt {
    ExprPtr expr; // the final expression

    template <typename T1, typename T2>
    FuncExprBlockStmt(T1&& stmts, T2&& expr)
        : BlockStmt(std::forward<T1>(stmts)), expr(std::forward<T2>(expr)) {}

    constexpr NodeType type() const override {
        return NodeType::FuncExprBlockStmt;
    }
};
using  FuncExprBlockStmtPtr = std::shared_ptr<FuncExprBlockStmt>;

void ast2Dot(std::ofstream& out, const ProgPtr p_prog);

} // namespace parser::ast
