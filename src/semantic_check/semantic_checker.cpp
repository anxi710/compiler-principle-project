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
}

void SemanticChecker::checkStmt(const StmtPtr& p_stmt)
{
}

}  // namespace semantic
