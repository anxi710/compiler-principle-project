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
    void checkProg(const parser::ast::ProgPtr& p_prog);

   private:
    void checkDecl(const parser::ast::DeclPtr& p_decl);
    void checkFuncDecl(const parser::ast::FuncDeclPtr& p_fdecl);
    void checkFuncHeaderDecl(const parser::ast::FuncHeaderDeclPtr& p_fhdecl);
    void checkBlockStmt(const parser::ast::BlockStmtPtr& p_bstmt);
    void checkStmt(const parser::ast::StmtPtr& p_stmt);

   private:
    std::shared_ptr<symbol::SymbolTable> p_stable;
    std::shared_ptr<error::ErrorReporter> p_ereporter;
};

}  // namespace semantic
