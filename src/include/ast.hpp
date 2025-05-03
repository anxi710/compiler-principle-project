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

// AST 结点
struct Node {
    virtual ~Node() = default;
};
using NodePtr = std::shared_ptr<Node>;

// Declaration
struct Decl : Node {
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
};
using  ProgPtr = std::shared_ptr<Prog>;

struct VarDecl : Decl {
    bool        mutable_; // mutable or not
    std::string name;     // variable name

    VarDecl() = default;
    VarDecl(bool mutable_, std::string name) : mutable_(mutable_), name(name) {}
};
using  VarDeclPtr = std::shared_ptr<VarDecl>;

struct VarType : Node {
    virtual ~VarType() = default;
};
using  VarTypePtr = std::shared_ptr<VarType>;

struct Integer : VarType {};
using  IntegerPtr = std::shared_ptr<Integer>;

struct Array : VarType {
    int cnt; // 数组中元素个数

    Array() = default;
    Array(int cnt) : cnt(cnt) {}
};
using  ArrayPtr = std::shared_ptr<Array>;

struct Tuple : VarType {
    int cnt; // 元组中元素个数

    Tuple() = default;
    Tuple(int cnt) : cnt(cnt) {}
};
using TuplePtr = std::shared_ptr<Tuple>;

// Argument
struct Arg : Node {
    VarDeclPtr var;  // variable
    VarTypePtr type; // variable type

    template<typename T1, typename T2>
    Arg(T1&& var, T2&& type) : var(std::forward<T1>(var)), type(std::forward<T2>(type)) {}
};
using  ArgPtr = std::shared_ptr<Arg>;

// Statement
struct Stmt : Node {
    virtual ~Stmt() = default;
};
using  StmtPtr = std::shared_ptr<Stmt>;

// Block statement
struct BlockStmt : Stmt {
    std::vector<StmtPtr> stmts; // statements

    template<typename T>
    BlockStmt(T&& stmts) : stmts(std::forward<T>(stmts)) {}
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
};
using  FuncDeclPtr = std::shared_ptr<FuncDecl>;

// Expression
struct Expr :Stmt {
    virtual ~Expr() = default;
};
using  ExprPtr = std::shared_ptr<Expr>;

struct Variable : Expr {
    std::string name; // 变量名

    Variable() = default;
    template<typename T>
    Variable(T&& name) : name(std::forward<T>(name)) {}
};
using  VariablePtr = std::shared_ptr<Variable>;

struct Number : Expr {
    int value; // 值

    Number() = default;
    Number(int value) : value(value) {}
};
using  NumberPtr = std::shared_ptr<Number>;

// Return Statement
struct RetStmt : Stmt {
    std::optional<ExprPtr> ret_val; // return value (an expression)

    template<typename T>
    RetStmt(T&& ret_val) : ret_val(std::forward<T>(ret_val)) {}
};
using  RetStmtPtr = std::shared_ptr<RetStmt>;

struct VarDeclStmt : Stmt, Decl {
    VarDeclPtr                var;  // variable
    std::optional<VarTypePtr> type; // variable type

    template<typename T1, typename T2>
    VarDeclStmt(T1&& var, T2&& type)
        : var(std::forward<T1>(var)), type(std::forward<T2>(type)) {}
};
using  VarDeclStmtPtr = std::shared_ptr<VarDeclStmt>;

struct AssignStmt : Stmt {
    std::string var;  // variable
    ExprPtr     expr; // expression

    template<typename T>
    AssignStmt(std::string var, T&& expr) : var(var), expr(std::forward<T>(expr)) {}
};
using  AssignStmtPtr = std::shared_ptr<AssignStmt>;

struct VarDeclAssignStmt : VarDeclStmt {
    ExprPtr expr;

    template<typename T1, typename T2, typename T3>
    VarDeclAssignStmt(T1&& var, T2&& type, T3&& expr)
        : VarDeclStmt(std::forward<T1>(var), std::forward<T2>(type)), expr(std::forward<T3>(expr)) {}
};
using  VarDeclAssignStmtPtr = std::shared_ptr<VarDeclAssignStmt>;

struct ArithmeticExpr : Expr {
    ExprPtr            lhs; // 左操作数
    lexer::token::Type op;  // operator
    ExprPtr            rhs; // 右操作数

    ArithmeticExpr(ExprPtr lhs, lexer::token::Type op, ExprPtr rhs)
        : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
};
using  ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

struct CallExpr : Expr {
    std::string          callee;
    std::vector<ExprPtr> arguments;

    template<typename T>
    CallExpr(std::string name, T&& args) : callee(name), arguments(std::forward<T>(args)) {}
};
using  CallExprPtr = std::shared_ptr<CallExpr>;

struct ElseClause : Stmt {
    std::optional<ExprPtr>  expr;  // else (if expr)?
    BlockStmtPtr            block;

    template<typename T1, typename T2>
    ElseClause(T1&& expr, T2&& block)
        : expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}
};
using  ElseClausePtr = std::shared_ptr<ElseClause>;

struct IfStmt : Stmt {
    ExprPtr                     expr;
    BlockStmtPtr                block;
    std::vector<ElseClausePtr>  elcls; // else clauses

    template<typename T1, typename T2, typename T3>
    IfStmt(T1&& expr, T2&& block, T3&& clauses): expr(std::forward<T1>(expr)),
        block(std::forward<T2>(block)), elcls(std::forward<T3>(clauses)) {}
};
using  IfStmtPtr = std::shared_ptr<IfStmt>;

struct WhileStmt : Stmt {
    ExprPtr         expr;
    BlockStmtPtr    block;

    template<typename T1, typename T2>
    WhileStmt(T1&& expr, T2&& block):
        expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}
};
using  WhileStmtPtr = std::shared_ptr<WhileStmt>;

struct ForStmt : Stmt {
    std::string name;
    ExprPtr     lexpr;
    ExprPtr     rexpr;
    BlockStmtPtr   block;

    template<typename T1, typename T2, typename T3>
    ForStmt(std::string name, T1&& expr1, T2&& expr2, T3&& block):
        name(std::move(name)), lexpr(std::forward<T1>(expr1)),
        rexpr(std::forward<T2>(expr2)), block(std::forward<T3>(block)){}
};
using  ForStmtPtr = std::shared_ptr<ForStmt>;

struct LoopStmt : Stmt {
    BlockStmtPtr    block;

    template<typename T>
    LoopStmt(T&& block) : block(std::forward<T>(block)){}
};
using  LoopStmtPtr = std::shared_ptr<LoopStmt>;

struct BreakStmt : Stmt {};
using  BreakStmtPtr = std::shared_ptr<BreakStmt>;

struct ContinueStmt : Stmt {};
using  ContinueStmtPtr = std::shared_ptr<ContinueStmt>;

struct NullStmt : Stmt {};
using  NullStmtPtr = std::shared_ptr<NullStmt>;

void ast2Dot(std::ofstream& out, const ProgPtr p_prog);

} // namespace parser::ast
