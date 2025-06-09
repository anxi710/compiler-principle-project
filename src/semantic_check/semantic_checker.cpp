#include "semantic_checker.hpp"

#include <cassert>

#include "../err_report/error_type.hpp"

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
    for (const auto& arg : p_fhdecl->argv)
    {
        std::string name = arg->variable->name;
        // 形参的初始值在函数调用时才会被赋予
        auto p_fparam = std::make_shared<symbol::Integer>(name, true, std::nullopt);
        p_stable->declareVar(name, p_fparam);
    }

    auto rt = p_fhdecl->retval_type.has_value() ? symbol::VarType::I32 : symbol::VarType::Null;

    auto p_func = std::make_shared<symbol::Function>(p_fhdecl->name, argc, rt);

    p_stable->declareFunc(p_fhdecl->name, p_func);
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

void SemanticChecker::checkVarDeclStmt(const VarDeclStmtPtr& p_vdstmt)
{
    // 需要实现的产生式中，所有变量声明语句声明的变量只有两种情况
    // 1. let mut a : i32;
    // 2. let mut a;
    // 因此，我们这里简单的记录变量
    // 需要注意的是，由于存在自动类型推导，如果没有指定类型，理论上只能声明一个 Variable
    // 符号，而不能直接声明一个 Variable 的子类
    // 简单起见，这里将所有变量声明为一个 i32 的变量

    // 变量二次声明时,直接将原变量覆盖
    auto p_var = std::make_shared<symbol::Integer>(p_vdstmt->variable->name, false, std::nullopt);
    p_stable->declareVar(p_var->name, p_var);
}

void SemanticChecker::checkRetStmt(const RetStmtPtr& p_rstmt)
{
    // 由于存在函数返回值类型的自动推导，因此需要在 RetStmt
    // 中判断所处函数是否指定了返回值类型，如果指定了与当前 RetStmt 的类型是否一致；
    // 如果未指定，则设置相应函数符号的返回值类型为 RetStmt 中返回值类型
    std::string cfunc_name = p_stable->getFuncName();
    auto opt_func = p_stable->lookupFunc(cfunc_name);
    assert(opt_func.has_value());

    const auto& p_func = opt_func.value();
    if (p_rstmt->ret_val.has_value())
    {  // 有返回值表达式
        symbol::VarType ret_type = checkExpr(p_rstmt->ret_val.value());

        if (p_func->retval_type != symbol::VarType::Null)
        {  // 函数有明确返回类型
            if (p_func->retval_type != ret_type)
            {  // 返回值表达式类型与函数返回类型不符
                p_ereporter->report(error::SemanticErrorType::FuncReturnTypeMismatch,
                                    std::format("函数 '{}' return 语句返回类型错误", cfunc_name),
                                    p_stable->getCurScope());
            }
        }
        else
        {  // 函数不需要返回值，却有返回值表达式
            p_ereporter->report(
                error::SemanticErrorType::VoidFuncReturnValue,
                std::format("函数 '{}' 不需要返回值，return语句却有返回值", cfunc_name),
                p_stable->getCurScope());
        }
    }
    else
    {  // return;
        if (p_func->retval_type != symbol::VarType::Null)
        {  // 有返回类型但无返回值表达式
            p_ereporter->report(
                error::SemanticErrorType::MissingReturnValue,
                std::format("函数 '{}' 不需要返回值，return语句却有返回值", cfunc_name),
                p_stable->getCurScope());
        }
    }
}

auto SemanticChecker::checkExprStmt(const ExprStmtPtr& p_estmt) -> symbol::VarType
{
    switch (p_estmt->expr->type())
    {
        default:
            return checkExpr(p_estmt->expr);
        case NodeType::CallExpr:
            return checkCallExpr(std::dynamic_pointer_cast<CallExpr>(p_estmt->expr));
    }
}

auto SemanticChecker::checkExpr(const ExprPtr& p_expr) -> symbol::VarType
{
    switch (p_expr->type())
    {
        default:
            throw std::runtime_error{"检查到不支持的表达式类型"};
        case NodeType::ParenthesisExpr:
        {
            auto paren_expr = std::dynamic_pointer_cast<ParenthesisExpr>(p_expr);
            return checkExpr(paren_expr->expr);
        }
        case NodeType::CallExpr:
            return checkCallExpr(std::dynamic_pointer_cast<CallExpr>(p_expr));
        case NodeType::ComparExpr:
            return checkComparExpr(std::dynamic_pointer_cast<ComparExpr>(p_expr));
        case NodeType::ArithExpr:
            return checkArithExpr(std::dynamic_pointer_cast<ArithExpr>(p_expr));
        case NodeType::Factor:
            return checkFactor(std::dynamic_pointer_cast<Factor>(p_expr));
        case NodeType::Variable:
            return checkVariable(std::dynamic_pointer_cast<Variable>(p_expr));
        case NodeType::Number:
            return checkNumber(std::dynamic_pointer_cast<Number>(p_expr));
    }
}

auto SemanticChecker::checkCallExpr(const CallExprPtr& p_caexpr) -> symbol::VarType
{
    // 检查调用表达式的实参和形参是否一致
    auto opt_func = p_stable->lookupFunc(p_caexpr->callee);

    if (!opt_func.has_value())
    {
        p_ereporter->report(error::SemanticErrorType::UndefinedFunctionCall,
                            std::format("调用了未定义的函数 '{}'", p_caexpr->callee),
                            p_stable->getCurScope());
        return symbol::VarType::Null;
    }

    const auto& p_func = opt_func.value();

    if (static_cast<int>(p_caexpr->argv.size()) != p_func->argc)
    {
        p_ereporter->report(error::SemanticErrorType::ArgCountMismatch,
                            std::format("函数 '{}' 期望 {} 个参数，但调用提供了 {} 个",
                                        p_caexpr->callee, p_func->argc, p_caexpr->argv.size()),
                            p_stable->getCurScope());
    }

    // 遍历所有实参与进行表达式语义检查
    for (const auto& arg : p_caexpr->argv)
    {
        assert(arg);
        auto rv_type = checkExpr(arg);
        if (rv_type != symbol::VarType::I32)
        {
            // error report
        }
    }

    return p_func->retval_type;
}

auto SemanticChecker::checkComparExpr(const ComparExprPtr& p_coexpr) -> symbol::VarType
{
    // 递归检查子树中所涉及的符号是否类型匹配，是否有值能够使用
    // 在这里我们只需要简单的检查，变量是否是第一次使用，如果是第一次使用，其是否有初值
    checkExprStmt(std::make_shared<ExprStmt>(p_coexpr->lhs));
    checkExprStmt(std::make_shared<ExprStmt>(p_coexpr->rhs));

    // 这里只是一个简化的实现，理论上应该是一个 Bool 类型的值
    return symbol::VarType::I32;
}

auto SemanticChecker::checkArithExpr(const ArithExprPtr& p_aexpr) -> symbol::VarType
{
    checkExprStmt(std::make_shared<ExprStmt>(p_aexpr->lhs));
    checkExprStmt(std::make_shared<ExprStmt>(p_aexpr->rhs));

    return symbol::VarType::I32;
}

auto SemanticChecker::checkFactor(const FactorPtr& p_factor) -> symbol::VarType
{
    auto expr = p_factor->element;

    switch (expr->type())
    {
        default:
            throw std::runtime_error{"不支持的因子类型"};
        case NodeType::Number:
            return checkNumber(std::dynamic_pointer_cast<Number>(p_factor->element));
        case NodeType::Variable:
            return checkVariable(std::dynamic_pointer_cast<Variable>(p_factor->element));
        case NodeType::CallExpr:
            return checkCallExpr(std::dynamic_pointer_cast<CallExpr>(p_factor->element));
    }
}

auto SemanticChecker::checkVariable(const VariablePtr& p_variable) -> symbol::VarType
{
    auto opt_var = p_stable->lookupVar(p_variable->name);

    if (!opt_var.has_value())
    {
        p_ereporter->report(error::SemanticErrorType::UndeclaredVariable,
                            std::format("变量 '{}' 未声明", p_variable->name),
                            p_stable->getCurScope());
        return symbol::VarType::Null;
    }

    const auto& p_var = opt_var.value();

    if (!p_var->initialized && !p_var->formal)
    {
        p_ereporter->report(error::SemanticErrorType::UninitializedVariable,
                            std::format("变量 '{}' 在第一次使用前未初始化", p_variable->name),
                            p_stable->getCurScope());
    }

    return p_var->var_type;
}

auto SemanticChecker::checkNumber(const NumberPtr& p_number) -> symbol::VarType
{
    return symbol::VarType::I32;  // 异常简化的实现
}

void SemanticChecker::checkAssignStmt(const AssignStmtPtr& p_astmt)
{
    auto lhs_var = std::dynamic_pointer_cast<Variable>(p_astmt->lvalue);

    if (!lhs_var)
    {
        p_ereporter->report(error::SemanticErrorType::AssignToNonVariable,
                            std::format("赋值语句左侧不是变量"), p_stable->getCurScope());
    }

    auto opt_var = p_stable->lookupVar(lhs_var->name);

    if (!opt_var.has_value())
    {
        p_ereporter->report(error::SemanticErrorType::AssignToUndeclaredVar,
                            std::format("赋值语句左侧变量 '{}' 未声明", lhs_var->name),
                            p_stable->getCurScope());
    }

    const auto& p_var = opt_var.value();

    // 先检查右侧表达式是否合法
    auto expr_stmt = std::make_shared<ExprStmt>(p_astmt->expr);
    checkExprStmt(expr_stmt);

    // 右侧合法后，标记左侧变量为“已初始化”
    p_var->initialized = true;
}

void SemanticChecker::checkIfStmt(const IfStmtPtr& p_istmt)
{
    checkExprStmt(std::make_shared<ExprStmt>(p_istmt->expr));
    checkBlockStmt(p_istmt->if_branch);

    assert(p_istmt->else_clauses.size() <= 1);
    if (p_istmt->else_clauses.size() == 1)
    {
        checkBlockStmt(p_istmt->else_clauses[0]->block);
    }
}

void SemanticChecker::checkWhileStmt(const WhileStmtPtr& p_wstmt)
{
    checkExprStmt(std::make_shared<ExprStmt>(p_wstmt->expr));
    checkBlockStmt(p_wstmt->block);
}

}  // namespace semantic
