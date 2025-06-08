#pragma once

#include <memory>

#include "../err_report/error_reporter.hpp"
#include "../parser/ast.hpp"
#include "symbol_table.hpp"

namespace semantic
{

class SemanticChecker
{
   public:
    SemanticChecker() = default;
    ~SemanticChecker() = default;

    void setErrorReporter(std::shared_ptr<error::ErrorReporter> p_ereporter);
    void setSymbolTable(std::shared_ptr<symbol::SymbolTable> p_stable);

   public:
    void checkProg(const parser::ast::ProgPtr& p_prog);
    auto getErrorReporter() { return p_ereporter; }

   private:
    void checkFuncDecl(const parser::ast::FuncDeclPtr& p_fdecl);
    void checkFuncHeaderDecl(const parser::ast::FuncHeaderDeclPtr& p_fhdecl);
    void checkBlockStmt(const parser::ast::BlockStmtPtr& p_bstmt);
    void checkVarDeclStmt(const parser::ast::VarDeclStmtPtr& p_vdstmt);
    void checkRetStmt(const parser::ast::RetStmtPtr& p_rstmt);
    auto checkExprStmt(const parser::ast::ExprStmtPtr& p_estmt) -> symbol::VarType;
    auto checkExpr(const parser::ast::ExprPtr& p_expr) -> symbol::VarType;
    auto checkCallExpr(const parser::ast::CallExprPtr& p_caexpr) -> symbol::VarType;
    auto checkComparExpr(const parser::ast::ComparExprPtr& p_coexpr) -> symbol::VarType;
    auto checkArithExpr(const parser::ast::ArithExprPtr& p_aexpr) -> symbol::VarType;
    auto checkFactor(const parser::ast::FactorPtr& p_factor) -> symbol::VarType;
    auto checkVariable(const parser::ast::VariablePtr& p_variable) -> symbol::VarType;
    auto checkNumber(const parser::ast::NumberPtr& p_number) -> symbol::VarType;
    void checkAssignStmt(const parser::ast::AssignStmtPtr& p_astmt);
    void checkIfStmt(const parser::ast::IfStmtPtr& p_istmt);
    void checkWhileStmt(const parser::ast::WhileStmtPtr& p_wstmt);

   private:
    std::shared_ptr<symbol::SymbolTable> p_stable;
    std::shared_ptr<error::ErrorReporter> p_ereporter;
};

}  // namespace semantic
