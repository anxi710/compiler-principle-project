#include "semantic_checker.hpp"

#include <cassert>

using namespace parser::ast;

namespace semantic
{

void SemanticChecker::setErrorReporter(std::shared_ptr<error::ErrorReporter> p_ereporter)
{
    this->p_ereporter = std::move(p_ereporter);
}

void SemanticChecker::setSymbolTable(std::shared_ptr<symbol::SymbolTable> p_stable)
{
    this->p_stable = std::move(p_stable);
}

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
    p_stable->enterScope(p_fdecl->header->name);  // 注意不要在没进入作用域时就开始声明变量！
    checkFuncHeaderDecl(p_fdecl->header);
    checkBlockStmt(p_fdecl->body);
    p_stable->exitScope();
}

void SemanticChecker::checkFuncHeaderDecl(const FuncHeaderDeclPtr& p_fhdecl)
{
    int argc = p_fhdecl->argv.size();
    try
    {
        for (const auto& arg : p_fhdecl->argv)
        {
            std::string name = arg->variable->name;
            auto p_fparam = std::make_shared<symbol::Integer>(
                name, true, std::nullopt);  // 形参的初始值在函数调用时才会被赋予
            p_stable->declareVar(name, p_fparam);
        }
    }
    catch (const std::exception& err)
    {
    }

    auto rt = p_fhdecl->retval_type.has_value() ? symbol::VarType::I32 : symbol::VarType::Null;

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
            case NodeType::NullStmt:
                break;
        }
    }
}

void SemanticChecker::checkVarDeclStmt(const parser::ast::VarDeclStmtPtr& p_vdstmt)
{
    // 需要实现的产生式中，所有变量声明语句声明的变量只有两种情况
    // 1. let mut a : i32;
    // 2. let mut a;
    // 因此，我们这里简单的记录变量
    // 需要注意的是，由于存在自动类型推导，如果没有指定类型，理论上只能声明一个 Variable
    // 符号，而不能直接声明一个 Variable 的子类
    // 简单起见，这里将所有变量声明为一个 i32 的变量

    // TODO: 这里需要补充对变量二次声明的检查
    auto p_var = std::make_shared<symbol::Integer>(p_vdstmt->variable->name, false, std::nullopt);
    p_stable->declareVar(p_var->name, p_var);
}

void SemanticChecker::checkRetStmt(const parser::ast::RetStmtPtr& p_rstmt)
{
    // 由于存在函数返回值类型的自动推导，因此需要在 RetStmt
    // 中判断所处函数是否指定了返回值类型，如果指定了与当前 RetStmt 的类型是否一致；
    // 如果未指定，则设置相应函数符号的返回值类型为 RetStmt 中返回值类型
}

void SemanticChecker::checkExprStmt(const parser::ast::ExprStmtPtr& p_estmt)
{
    switch (p_estmt->expr->type())
    {
        default:
            throw std::runtime_error{"检查到不支持的表达式类型"};
        case NodeType::CallExpr:
            checkCallExpr(std::dynamic_pointer_cast<CallExpr>(p_estmt->expr));
            break;
        case NodeType::ComparExpr:
            checkComparExpr(std::dynamic_pointer_cast<ComparExpr>(p_estmt->expr));
            break;
    }
}

void SemanticChecker::checkCallExpr(const parser::ast::CallExprPtr& p_caexpr)
{
    // 检查调用表达式的实参和形参是否一致
}

void SemanticChecker::checkComparExpr(const parser::ast::ComparExprPtr& p_coexpr)
{
    // 递归检查子树中所涉及的符号是否类型匹配，是否有值能够使用
    // 在这里我们只需要简单的检查，变量是否是第一次使用，如果是第一次使用，其是否有初值
}

void SemanticChecker::checkAssignStmt(const parser::ast::AssignStmtPtr& p_astmt)
{
    // 这里可以简化一点，只检查变量是否有初值
}

void SemanticChecker::checkIfStmt(const parser::ast::IfStmtPtr& p_istmt)
{
}

void SemanticChecker::checkWhileStmt(const parser::ast::WhileStmtPtr& p_wstmt)
{
}

}  // namespace semantic
