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

   private:
    void checkFuncDecl(const parser::ast::FuncDeclPtr& p_fdecl);
    void checkFuncHeaderDecl(const parser::ast::FuncHeaderDeclPtr& p_fhdecl);
    void checkBlockStmt(const parser::ast::BlockStmtPtr& p_bstmt);
    void checkVarDeclStmt(const parser::ast::VarDeclStmtPtr& p_vdstmt);
    void checkRetStmt(const parser::ast::RetStmtPtr& p_rstmt);
    void checkExprStmt(const parser::ast::ExprStmtPtr& p_estmt);
    void checkCallExpr(const parser::ast::CallExprPtr& p_caexpr);
    void checkComparExpr(const parser::ast::ComparExprPtr& p_coexpr);
    void checkAssignStmt(const parser::ast::AssignStmtPtr& p_astmt);
    void checkIfStmt(const parser::ast::IfStmtPtr& p_istmt);
    void checkWhileStmt(const parser::ast::WhileStmtPtr& p_wstmt);

   private:
    std::shared_ptr<symbol::SymbolTable> p_stable;
    std::shared_ptr<error::ErrorReporter> p_ereporter;
};

}  // namespace semantic
