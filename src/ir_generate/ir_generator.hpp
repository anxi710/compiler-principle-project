#pragma once

#include <vector>

#include "../parser/ast.hpp"
#include "../semantic_check/symbol_table.hpp"

namespace ir
{

enum class OpCode : std::int8_t
{
    Add,
    Sub,
    Mul,
    Div,

    Jeq,
    Jne,
    Jge,
    Jgt,
    Jle,
    Jlt,

    Eq,
    Neq,
    Geq,
    Gne,
    Leq,
    Lne,

    Decl,
    Assign,

    Label,
    Goto,

    Push,
    Pop,
    Call,
    Return
};

struct Operand
{
    std::string name;

    Operand() = default;
    Operand(std::string name) : name(std::move(name)) {}
};

struct Quad
{
    OpCode op;
    Operand arg1;
    Operand arg2;
    Operand res;
};

class IrGenerator
{
   public:
    IrGenerator() = default;
    ~IrGenerator() = default;

    void setSymbolTable(std::shared_ptr<symbol::SymbolTable> p_stable);

   public:
    void generateProg(const parser::ast::ProgPtr& p_prog);

    void printQuads(std::ofstream& out) const;

   private:
    void generateFuncDecl(const parser::ast::FuncDeclPtr& p_fdecl);
    void generateFuncHeaderDecl(const parser::ast::FuncHeaderDeclPtr& p_fhdecl);
    void generateBlockStmt(const parser::ast::BlockStmtPtr& p_bstmt);
    void generateVarDeclStmt(const parser::ast::VarDeclStmtPtr& p_vdstmt);
    void generateRetStmt(const parser::ast::RetStmtPtr& p_rstmt);
    void generateExprStmt(const parser::ast::ExprStmtPtr& p_estmt);
    auto generateExpr(const parser::ast::ExprPtr& p_expr) -> std::string;
    auto generateCallExpr(const parser::ast::CallExprPtr& p_caexpr) -> std::string;
    auto generateComparExpr(const parser::ast::ComparExprPtr& p_coexpr) -> std::string;
    auto generateArithExpr(const parser::ast::ArithExprPtr& p_aexpr) -> std::string;
    auto generateFactor(const parser::ast::FactorPtr& p_factor) -> std::string;
    auto generateElement(const parser::ast::ExprPtr& p_element) -> std::string;
    auto generateParenthesisExpr(const parser::ast::ParenthesisExprPtr& p_pexpr) -> std::string;
    static auto generateNumber(const parser::ast::NumberPtr& p_number) -> std::string;
    auto generateVariable(const parser::ast::VariablePtr& p_variable) -> std::string;
    void generateAssignStmt(const parser::ast::AssignStmtPtr& p_astmt);
    void generateIfStmt(const parser::ast::IfStmtPtr& p_istmt);
    void generateWhileStmt(const parser::ast::WhileStmtPtr& p_wstmt);

    [[nodiscard]]
    auto getVarName(const std::string& var_name) const -> std::string;
    [[nodiscard]]
    auto getFuncName() const -> std::string;

    void pushQuads(OpCode op, const Operand& arg1, const Operand& arg2, const Operand& res);

   private:
    std::vector<Quad> quads;

    std::shared_ptr<symbol::SymbolTable> p_stable;
};

}  // namespace ir
