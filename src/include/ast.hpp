#pragma once

#include <vector>
#include <memory>
#include <string>

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
    std::string name; // argument name

    template<typename T>
    Arg(T&& name) : name(std::forward<T>(name)) {} // T&& 是一个通用引用
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
    std::string         name; // function name
    std::vector<ArgPtr> argv; // argument vector

    template<typename T1, typename T2>
    FuncHeaderDecl(T1&& name, T2&& argv)
        : name(std::forward<T1>(name)), argv(std::forward<T2>(argv)) {}
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

} // namespace parser::ast
