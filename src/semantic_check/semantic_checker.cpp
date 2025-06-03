#include "semantic_checker.hpp"

#include <cassert>

using namespace parser::ast;

namespace semantic
{

void SemanticChecker::checkProg(const ProgPtr& p_prog)
{
    for (const auto& p_decl : p_prog->decls)
    {
        // 最顶层的产生式为 Prog -> (FuncDecl)*
        auto p_fdecl = std::dynamic_pointer_cast<FuncDecl>(p_decl);
        assert(p_fdecl);  // 在 parser 的实现中，只能解析 FuncDecl，碰到非函数声明会直接终止解析！
        checkFuncDecl(p_fdecl);
    }
}

void SemanticChecker::checkFuncDecl(const FuncDeclPtr& p_fdecl)
{
    checkFuncHeaderDecl(p_fdecl->header);
    p_stable->enterScope(p_fdecl->header->name);
    checkBlockStmt(p_fdecl->body);
    p_stable->exitScope();
}

void SemanticChecker::checkFuncHeaderDecl(const FuncHeaderDeclPtr& p_fhdecl)
{
    int argc = p_fhdecl->argv.size();
    for (const auto& arg : p_fhdecl->argv)
    {
        std::string name = arg->variable->name;
        auto p_fparam = std::make_shared<symbol::Integer>(
            name, true, std::nullopt);  // 形参的初始值在函数调用时才会被赋予
        p_stable->declareVar(name, p_fparam);
    }

    symbol::VarType rt =
        p_fhdecl->retval_type.has_value() ? symbol::VarType::I32 : symbol::VarType::Null;

    auto p_func = std::make_shared<symbol::Function>(p_fhdecl->name, argc, rt);

    try
    {
        p_stable->declareFunc(p_fhdecl->name, p_func);
    }
    catch (const std::exception& err)
    {
    }
}

void SemanticChecker::checkBlockStmt(const BlockStmtPtr& p_bstmt)
{
    int if_cnt = 1;
    int while_cnt = 1;

    for (const auto& p_stmt : p_bstmt->stmts)
    {
        switch (p_stmt->type())
        {
            default:
                throw std::runtime_error{"检查到不支持的语句类型"};
            case NodeType::VarDeclStmt:
                checkVarDeclStmt(std::dynamic_pointer_cast<VarDeclStmt>(p_stmt));
                break;
            case NodeType::RetStmt:
                checkRetStmt(std::dynamic_pointer_cast<RetStmt>(p_stmt));
                break;
            case NodeType::ExprStmt:
                checkExprStmt(std::dynamic_pointer_cast<ExprStmt>(p_stmt));
                break;
            case NodeType::AssignStmt:
                checkAssignStmt(std::dynamic_pointer_cast<AssignStmt>(p_stmt));
                break;
            case NodeType::IfStmt:
                p_stable->enterScope(std::format("if{}", if_cnt++));
                checkIfStmt(std::dynamic_pointer_cast<IfStmt>(p_stmt));
                p_stable->exitScope();
                break;
            case NodeType::WhileStmt:
                p_stable->enterScope(std::format("while{}", while_cnt++));
                checkWhileStmt(std::dynamic_pointer_cast<WhileStmt>(p_stmt));
                p_stable->exitScope();
                break;
        }
    }
}

void SemanticChecker::checkVarDeclStmt(const parser::ast::VarDeclStmtPtr& p_vdstmt)
{
}

void SemanticChecker::checkRetStmt(const parser::ast::RetStmtPtr& p_rstmt)
{
}

void SemanticChecker::checkExprStmt(const parser::ast::ExprStmtPtr& p_estmt)
{
}

void SemanticChecker::checkAssignStmt(const parser::ast::AssignStmtPtr& p_astmt)
{
}

void SemanticChecker::checkIfStmt(const parser::ast::IfStmtPtr& p_istmt)
{
}

void SemanticChecker::checkWhileStmt(const parser::ast::WhileStmtPtr& p_wstmt)
{
}

}  // namespace semantic
