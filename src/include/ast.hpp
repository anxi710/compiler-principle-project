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

// Argument
struct Arg : Node {
    std::string name;

    Arg(std::string name) : name(name) {}
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
    std::string                       name;        // function name
    std::vector<ArgPtr>               argv;        // argument vector
    std::optional<lexer::token::Type> retval_type; // return value type

    template<typename T1, typename T2>
    FuncHeaderDecl(T1&& name, T2&& argv, std::optional<lexer::token::Type> retval_type)
        : name(std::forward<T1>(name)), argv(std::forward<T2>(argv)), retval_type(retval_type) {}
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

// Return Statement
struct RetStmt : Stmt {
    std::optional<ExprPtr> ret_val; // return value (an expression)

    template<typename T>
    RetStmt(T&& ret_val) : ret_val(std::forward<T>(ret_val)) {}
};
using  RetStmtPtr = std::shared_ptr<RetStmt>;

struct VarDeclStmt : Stmt, Decl {
    bool                               mutable_; // mutable 是一个保留字
    lexer::token::Token                id;       // variable (identifier)
    std::optional<lexer::token::Type>  type;     // variable type

    template<typename T>
    VarDeclStmt(bool mutable_, T&& id, std::optional<lexer::token::Type> type)
        : mutable_(mutable_), id(std::forward<T>(id)), type(type) {}
};
using  VarDeclStmtPtr = std::shared_ptr<VarDeclStmt>;

struct AssignStmt : Stmt {
    std::string name;
    ExprPtr     expr;

    template<typename T>
    AssignStmt(std::string name, T&& expr) : name(name), expr(std::forward<T>(expr)) {}
};
using  AssignStmtPtr = std::shared_ptr<AssignStmt>;

struct VarDeclAssignStmt : VarDeclStmt {
    ExprPtr expr;

    template<typename T>
    VarDeclAssignStmt(bool mutable_, lexer::token::Token id, std::optional<lexer::token::Type> type, T&& expr)
        : VarDeclStmt(mutable_, std::move(id), type), expr(std::forward<T>(expr)) {}
};
using  VarDeclAssignStmtPtr = std::shared_ptr<VarDeclAssignStmt>;

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

struct ArithmeticExpr : Expr {
    ExprPtr            lhs; // 左操作数
    lexer::token::Type op;  // operator
    ExprPtr            rhs; // 右操作数

    ArithmeticExpr(ExprPtr lhs, lexer::token::Type op, ExprPtr rhs)
        : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
};
using ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

struct CallExpr : Expr {
    std::string          callee;
    std::vector<ExprPtr> arguments;

    CallExpr(std::string name, std::vector<ExprPtr> args)
        : callee(std::move(name)), arguments(std::move(args)) {}
};
using CallExprPtr = std::shared_ptr<CallExpr>;


struct ElseClause : Stmt{
    std::optional<ExprPtr>  expr;
    BlockStmtPtr            block;

    template<typename T>
    ElseClause(T&& expr, BlockStmtPtr block)
        : expr(std::forward<T>(expr)), block(std::move(block)) {}
    
    ElseClause(std::nullopt_t, BlockStmtPtr block)
        : expr(std::nullopt), block(std::move(block)) {}

};
using ElseClausePtr = std::shared_ptr<ElseClause>;


struct IfStmt : Stmt{
    ExprPtr                     expr;
    BlockStmtPtr                block;
    std::vector<ElseClausePtr>  elcls;
    template<typename T>
    IfStmt(T&& expr,BlockStmtPtr block,std::vector<ElseClausePtr> clauses):
        expr(std::forward<T>(expr)),block(std::move(block)),elcls(std::move(clauses)){}
};
using IfStmtPtr = std::shared_ptr<IfStmt>;


struct WhileStmt : Stmt{
    ExprPtr         expr;
    BlockStmtPtr    block;
    template<typename T>
    WhileStmt(T&& expr,BlockStmtPtr block):
        expr(std::forward<T>(expr)),block(std::move(block)){}
};
using WhileStmtPtr = std::shared_ptr<WhileStmt>;

struct ForStmt : Stmt{
    std::string name;
    ExprPtr     lexpr;
    ExprPtr     rexpr;
    BlockStmtPtr   block;
    template<typename T1,typename T2>
    ForStmt(std::string name,T1&& expr1,T2&& expr2,BlockStmtPtr block):
        name(std::move(name)),lexpr(std::forward<T1>(expr1)),rexpr(std::forward<T2>(expr2)),block(std::move(block)){}
};
using ForStmtPtr = std::shared_ptr<ForStmt>;


struct LoopStmt : Stmt{
    BlockStmtPtr    block;
    LoopStmt(BlockStmtPtr block):block(std::move(block)){}
};
using LoopStmtPtr = std::shared_ptr<LoopStmt>;

void ast2Dot(std::ofstream& out, const ProgPtr p_prog);

} // namespace parser::ast
