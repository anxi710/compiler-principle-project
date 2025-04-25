#include <vector>
#include <cassert>
#include <iostream>

#include "../src/include/ast.hpp"

// 多态与 RTTI (运行时类型识别)
int main() {
    using namespace parser::ast;

    std::string         name = "test";
    std::vector<ArgPtr> argv {};
    FuncHeaderDeclPtr func_header = std::make_shared<FuncHeaderDecl>(name, argv);

    std::vector<StmtPtr> stmts {};
    BlockStmtPtr block_stmt = std::make_shared<BlockStmt>(stmts);

    FuncDeclPtr func_decl = std::make_shared<FuncDecl>(func_header, block_stmt);

    DeclPtr decl = func_header;

    // 对于 RTTI，使用 dynamic_cast 来判断具体类型
    // 使用时，基类 Decl 需要包含至少一个虚函数（虚析构函数也行），否则 dynamic_cast 不起作用
    assert(!std::dynamic_pointer_cast<FuncDecl>(decl));
    if (std::dynamic_pointer_cast<FuncHeaderDecl>(decl)) {
        std::cout << "This is a FuncHeaderDecl" << std::endl;
    }

    decl = func_decl;
    assert(!std::dynamic_pointer_cast<FuncHeaderDecl>(decl));
    if (std::dynamic_pointer_cast<FuncDecl>(decl)) {
        std::cout << "This is a FuncDecl" << std::endl;
    }

    return 0;
}
