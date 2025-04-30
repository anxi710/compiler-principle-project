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
struct Expr {
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
    std::string name;

    Variable() = default;
    template<typename T>
    Variable(T&& name) : name(std::forward<T>(name)) {}
};
using  VariablePtr = std::shared_ptr<Variable>;

struct Number : Expr {
    int value;

    Number() = default;
    Number(int value) : value(value) {}
};
using  NumberPtr = std::shared_ptr<Number>;

struct ArithmeticExpr : Expr {
    ExprPtr            lhs; // 左操作数
    lexer::token::Type op;  // operator
    ExprPtr            rhs; // 右操作数
};
using ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

void ast2Dot(std::ofstream& out, const ProgPtr p_prog);

} // namespace parser::ast
