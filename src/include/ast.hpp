#pragma once

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <optional>
#include "token.hpp"

namespace parser::ast {

// 如果基类析构函数没有被声明为虚函数，则 C++ 只会调用基类的析构函数，
// 而不会调用派生类的析构函数

enum class NodeType {
    Prog, Arg,

    Decl, Stmt, Expr, VarType,

    VarDecl, FuncDecl, FuncHeaderDecl,

    BlockStmt, ExprStmt, RetStmt, VarDeclStmt, AssignStmt,
    VarDeclAssignStmt, ElseClause, IfStmt, WhileStmt, ForStmt,
    LoopStmt, BreakStmt, ContinueStmt, NullStmt,

    Variable, Number, Factor, ArithmeticExpr, CallExpr,
    FuncExprBlockStmt, IfExpr, ArrayElements, LValue,

    Integer, Array, Tuple
};

// 所有 AST 结点的基类
struct Node {
    virtual constexpr NodeType type() const = 0;
    virtual ~Node() = default;
};
using  NodePtr = std::shared_ptr<Node>;

// Declaration
struct Decl : Node {
    constexpr NodeType type() const override { return NodeType::Decl; }
    virtual ~Decl() = default;
};
using  DeclPtr = std::shared_ptr<Decl>;

// Program
struct Prog : Node {
    std::vector<DeclPtr> decls; // declarations

    Prog() = default;
    explicit Prog(const std::vector<DeclPtr>& ds) : decls(ds) {}
    explicit Prog(std::vector<DeclPtr>&& ds) : decls(std::move(ds)) {}

    constexpr NodeType type() const override { return NodeType::Prog; }
};
using  ProgPtr = std::shared_ptr<Prog>;

// Variable Declaration
struct VarDecl : Decl {
    bool        mut = true; // mutable or not
    std::string name;       // variable name

    VarDecl() = default;
    explicit VarDecl(bool mut, const std::string& n) : mut(mut), name(n) {}
    explicit VarDecl(bool mut, std::string&& n) : mut(mut), name(n) {}

    constexpr NodeType type() const override { return NodeType::VarDecl; }
};
using  VarDeclPtr = std::shared_ptr<VarDecl>;

// 类型引用修饰符
enum class RefType {
    Normal,    // 普通变量
    Deref,     // 解引用
    Immutable, // 不可变引用
    Mutable    // 可变引用
};

// Variable Type
struct VarType : Node {
    RefType ref_type = RefType::Normal;

    VarType() = default;
    explicit VarType(RefType rt) : ref_type(rt) {}
    virtual ~VarType() = default;

    constexpr NodeType type() const override { return NodeType::VarType; }
};
using  VarTypePtr = std::shared_ptr<VarType>;

struct Integer : VarType {
    Integer() = default;
    explicit Integer(RefType rt) : VarType(rt) {}

    constexpr NodeType type() const override { return NodeType::Integer; }
};
using  IntegerPtr = std::shared_ptr<Integer>;

struct Array : VarType {
    int         cnt = 0; // 数组中元素个数
    VarTypePtr  elementType;
    Array() = default;
    explicit Array(int cnt,VarTypePtr elementType, RefType rt = RefType::Normal) : VarType(rt), cnt(cnt),elementType(elementType) {}

    constexpr NodeType type() const override { return NodeType::Array; }
};
using  ArrayPtr = std::shared_ptr<Array>;

struct Tuple : VarType {
    int cnt = 0; // 元组中元素个数

    Tuple() = default;
    explicit Tuple(int cnt, RefType rt = RefType::Normal) : VarType(rt), cnt(cnt) {}

    constexpr NodeType type() const override { return NodeType::Tuple; }
};
using TuplePtr = std::shared_ptr<Tuple>;

// Argument
struct Arg : Node {
    VarDeclPtr variable;  // variable
    VarTypePtr var_type; // variable type

    Arg() = default;
    explicit Arg(const VarDeclPtr& var, const VarTypePtr& vt) : variable(var), var_type(vt) {}
    explicit Arg(VarDeclPtr&& var, VarTypePtr&& vt)
        : variable(std::move(var)), var_type(std::move(vt)) {}

    constexpr NodeType type() const override { return NodeType::Arg; }
};
using  ArgPtr = std::shared_ptr<Arg>;

// Statement
struct Stmt : Node {
    virtual ~Stmt() = default;
    constexpr NodeType type() const override { return NodeType::Stmt; }
};
using  StmtPtr = std::shared_ptr<Stmt>;

// Block statement
struct BlockStmt : Stmt {
    std::vector<StmtPtr> stmts; // statements

    BlockStmt() = default;
    explicit BlockStmt(const std::vector<StmtPtr>& s) : stmts(s) {}
    explicit BlockStmt(std::vector<StmtPtr>&& s) : stmts(std::move(s)) {}

    constexpr NodeType type() const override { return NodeType::BlockStmt; }
};
using  BlockStmtPtr = std::shared_ptr<BlockStmt>;

// Function header declaration
struct FuncHeaderDecl : Decl {
    std::string               name;        // function name
    std::vector<ArgPtr>       argv;        // argument vector
    std::optional<VarTypePtr> retval_type; // return value type

    FuncHeaderDecl() = default;
    explicit FuncHeaderDecl(const std::string& n, const std::vector<ArgPtr>& av, const std::optional<VarTypePtr>& rt)
        : name(n), argv(av), retval_type(rt) {};
    explicit FuncHeaderDecl(std::string&& n, std::vector<ArgPtr>&& av, std::optional<VarTypePtr>&& rt)
        : name(std::move(n)), argv(std::move(av)), retval_type(std::move(rt)) {}

    constexpr NodeType type() const override { return NodeType::FuncHeaderDecl; }
};
using  FuncHeaderDeclPtr = std::shared_ptr<FuncHeaderDecl>;

// Function Declaration
struct FuncDecl : Decl {
    FuncHeaderDeclPtr header; // function header
    BlockStmtPtr      body;   // function body

    FuncDecl() = default;
    explicit FuncDecl(const FuncHeaderDeclPtr& h, const BlockStmtPtr& b)
        : header(h), body(b) {}
    explicit FuncDecl(FuncHeaderDeclPtr&& h, BlockStmtPtr&& b)
        : header(std::move(h)), body(std::move(b)) {}

    constexpr NodeType type() const override { return NodeType::FuncDecl; }
};
using  FuncDeclPtr = std::shared_ptr<FuncDecl>;

// Expression
struct Expr : Node {
    virtual ~Expr() = default;
    constexpr NodeType type() const override { return NodeType::Expr; }
};
using  ExprPtr = std::shared_ptr<Expr>;

// Expression Statement
struct ExprStmt : Stmt {
    ExprPtr expr;

    ExprStmt() = default;
    explicit ExprStmt(const ExprPtr& e) : expr(e) {}
    explicit ExprStmt(ExprPtr&& e) : expr(std::move(e)) {}

    constexpr NodeType type() const override { return NodeType::ExprStmt; }
};
using  ExprStmtPtr = std::shared_ptr<ExprStmt>;

//a mixture of 4 kind of LValue
struct LValue : Expr{
    enum class Kind{
        Variable,   //普通变量 x
        Dereference,//解引用 *x
        ArrayAccess,//数组访问 a[i]
        TupleAccess //元组访问 t.0
    };
    Kind kind;
    virtual ~LValue() = default;

    constexpr NodeType type() const override{ return NodeType::LValue;}
};
using LValuePtr = std::shared_ptr<LValue>;

struct Variable : LValue {
    std::string name; // 变量名

    Variable() = default;
    explicit Variable(const std::string& n) : name(n) {kind = Kind::Variable;}
    explicit Variable(std::string&& n) : name(std::move(n)) {kind = Kind::Variable;}

    constexpr NodeType type() const override { return NodeType::Variable; }
};
using  VariablePtr = std::shared_ptr<Variable>;

struct Dereference : LValue{
    std::string target; //被解引用的变量x

    explicit Dereference(std::string&& target):target(std::move(target)){kind = Kind::Dereference;}
};
using DereferencePtr = std::shared_ptr<Dereference>;

struct ArrayAccess : LValue{
    std::string array;
    ExprPtr     index;
    explicit ArrayAccess(std::string&& array, ExprPtr&& index)
    : array(std::move(array)), index(std::move(index)) {
        kind = Kind::ArrayAccess;
    }
};
using ArrayAccessPtr = std::shared_ptr<ArrayAccess>;

struct TupleAccess : LValue{
    std::string tuple;
    int         index;

    explicit TupleAccess(std::string&& tuple, int index)
    : tuple(std::move(tuple)), index(index) {
        kind = Kind::TupleAccess;
    }
};
using TupleAccessPtr = std::shared_ptr<TupleAccess>;

struct Number : Expr {
    int value; // 值

    Number() = default;
    Number(int value) : value(value) {}

    constexpr NodeType type() const override { return NodeType::Number; }
};
using  NumberPtr = std::shared_ptr<Number>;

struct Factor : Expr {
    RefType ref_type;
    ExprPtr element;

    Factor() = default;
    explicit Factor(const RefType& rt, const ExprPtr& e) : ref_type(rt), element(e) {}
    explicit Factor(RefType&& rt, ExprPtr&& e) : ref_type(std::move(rt)), element(std::move(e)) {}

    constexpr NodeType type() const override { return NodeType::Factor; }
};
using  FactorPtr = std::shared_ptr<Factor>;

struct ArrayElements : Expr{
    std::vector<ExprPtr> elements;

    ArrayElements() = default;

    explicit ArrayElements(const std::vector<ExprPtr>& els):elements(els) {}
    explicit ArrayElements(std::vector<ExprPtr>&& els):elements(std::move(els)) {}

    constexpr NodeType type() const override { return NodeType::ArrayElements; }
};
using ArrayElementsPtr = std::shared_ptr<ArrayElements>;

// Return Statement
struct RetStmt : Stmt {
    std::optional<ExprPtr> ret_val; // return value (an expression)

    RetStmt() = default;
    explicit RetStmt(const std::optional<ExprPtr>& rv) : ret_val(rv) {}
    explicit RetStmt(std::optional<ExprPtr>&& rv) : ret_val(std::move(rv)) {}

    constexpr NodeType type() const override { return NodeType::RetStmt; }
};
using  RetStmtPtr = std::shared_ptr<RetStmt>;

// Variable Declaration Statement
struct VarDeclStmt : Stmt, Decl {
    VarDeclPtr                variable; // variable
    std::optional<VarTypePtr> var_type; // variable type

    VarDeclStmt() = default;
    explicit VarDeclStmt(const VarDeclPtr& var, const std::optional<VarTypePtr>& vt)
        : variable(var), var_type(vt) {}
    explicit VarDeclStmt(VarDeclPtr&& var, std::optional<VarTypePtr>&& vt)
        : variable(std::move(var)), var_type(std::move(vt)) {}

    constexpr NodeType type() const override { return NodeType::VarDeclStmt; }
};
using  VarDeclStmtPtr = std::shared_ptr<VarDeclStmt>;

// Assign Statement
struct AssignStmt : Stmt {
    LValuePtr   lv;     // four kind
    ExprPtr     expr;   // expression

    AssignStmt() = default;
    explicit AssignStmt(const LValuePtr& lv, const ExprPtr& e)
        : lv(lv), expr(e) {}
    explicit AssignStmt(LValuePtr lv, ExprPtr&& e)
        : lv(std::move(lv)), expr(std::move(e)) {}

    constexpr NodeType type() const override { return NodeType::AssignStmt; }
};
using  AssignStmtPtr = std::shared_ptr<AssignStmt>;

// Variable Declaration Assign Statement
struct VarDeclAssignStmt : VarDeclStmt {
    ExprPtr expr;

    template<typename T1, typename T2, typename T3>
    VarDeclAssignStmt(T1&& var, T2&& type, T3&& expr)
        : VarDeclStmt(std::forward<T1>(var), std::forward<T2>(type)), expr(std::forward<T3>(expr)) {}

    constexpr NodeType type() const override {
        return NodeType::VarDeclAssignStmt;
    }
};
using  VarDeclAssignStmtPtr = std::shared_ptr<VarDeclAssignStmt>;

// Arithmetic Expression
struct ArithmeticExpr : Expr {
    ExprPtr            lhs; // 左操作数
    lexer::token::Type op;  // operator
    ExprPtr            rhs; // 右操作数

    ArithmeticExpr() = default;
    explicit ArithmeticExpr(const ExprPtr& l, const lexer::token::Type& op, const ExprPtr& r)
        : lhs(l), op(op), rhs(r) {}
    explicit ArithmeticExpr(ExprPtr&& l, const lexer::token::Type& op, ExprPtr&& r)
        : lhs(std::move(l)), op(op), rhs(std::move(r)) {}

    constexpr NodeType type() const override { return NodeType::ArithmeticExpr; }
};
using  ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

// Call Expression
struct CallExpr : Expr {
    std::string          callee; // 被调用函数名
    std::vector<ExprPtr> argv;   // argument vector

    CallExpr() = default;
    explicit CallExpr(const std::string& ce, const std::vector<ExprPtr>& av)
        : callee(ce), argv(av) {}
    explicit CallExpr(std::string&& ce, std::vector<ExprPtr>&& argv)
        : callee(std::move(ce)), argv(std::move(argv)) {}

    constexpr NodeType type() const override { return NodeType::CallExpr; }
};
using  CallExprPtr = std::shared_ptr<CallExpr>;

struct ElseClause : Stmt {
    std::optional<ExprPtr>  expr;  // else (if expr)?
    BlockStmtPtr            block;

    template<typename T1, typename T2>
    ElseClause(T1&& expr, T2&& block)
        : expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}

    constexpr NodeType type() const override { return NodeType::ElseClause; }
};
using  ElseClausePtr = std::shared_ptr<ElseClause>;

struct IfStmt : Stmt {
    ExprPtr                     expr;
    BlockStmtPtr                if_branch;
    std::vector<ElseClausePtr>  else_clauses; // else clauses

    template<typename T1, typename T2, typename T3>
    IfStmt(T1&& e, T2&& ib, T3&& clauses) : expr(std::forward<T1>(e)),
        if_branch(std::forward<T2>(ib)), else_clauses(std::forward<T3>(clauses)) {}

    constexpr NodeType type() const override { return NodeType::IfStmt; }
};
using  IfStmtPtr = std::shared_ptr<IfStmt>;

struct WhileStmt : Stmt {
    ExprPtr         expr;
    BlockStmtPtr    block;

    template<typename T1, typename T2>
    WhileStmt(T1&& expr, T2&& block)
        : expr(std::forward<T1>(expr)), block(std::forward<T2>(block)) {}

    constexpr NodeType type() const override { return NodeType::WhileStmt; }
};
using  WhileStmtPtr = std::shared_ptr<WhileStmt>;

struct ForStmt : Stmt {
    VarDeclPtr   var;
    ExprPtr      lexpr;
    ExprPtr      rexpr;
    BlockStmtPtr block;

    template<typename T, typename T1, typename T2, typename T3>
    ForStmt(T&& var, T1&& expr1, T2&& expr2, T3&& block) :
        var(std::forward<T>(var)), lexpr(std::forward<T1>(expr1)),
        rexpr(std::forward<T2>(expr2)), block(std::forward<T3>(block)){}

    constexpr NodeType type() const override { return NodeType::ForStmt; }
};
using  ForStmtPtr = std::shared_ptr<ForStmt>;

struct LoopStmt : Stmt, Expr {
    BlockStmtPtr    block;

    LoopStmt() = default;
    explicit LoopStmt(const BlockStmtPtr& b) : block(b) {}
    explicit LoopStmt(BlockStmtPtr&& b) : block(std::move(b)){}

    constexpr NodeType type() const override { return NodeType::LoopStmt; }
};
using  LoopStmtPtr = std::shared_ptr<LoopStmt>;

struct BreakStmt : Stmt {
    std::optional<ExprPtr> expr;

    BreakStmt() = default;
    explicit BreakStmt(const std::optional<ExprPtr>& e) : expr(e) {}
    explicit BreakStmt(std::optional<ExprPtr>&& e) : expr(std::move(e)) {}
    ~BreakStmt() override = default;

    constexpr NodeType type() const override { return NodeType::BreakStmt; }
};
using  BreakStmtPtr = std::shared_ptr<BreakStmt>;

struct ContinueStmt : Stmt {
    ~ContinueStmt() override = default;
    constexpr NodeType type() const override { return NodeType::ContinueStmt; }
};
using  ContinueStmtPtr = std::shared_ptr<ContinueStmt>;

struct NullStmt : Stmt {
    ~NullStmt() override = default;
    constexpr NodeType type() const override { return NodeType::NullStmt; }
};
using  NullStmtPtr = std::shared_ptr<NullStmt>;

// Function Expression Block Statement
struct FuncExprBlockStmt : Expr, BlockStmt {
    ExprPtr expr; // the final expression

    template <typename T1, typename T2>
    FuncExprBlockStmt(T1&& stmts, T2&& expr)
        : BlockStmt(std::forward<T1>(stmts)), expr(std::forward<T2>(expr)) {}

    constexpr NodeType type() const override { return NodeType::FuncExprBlockStmt; }
};
using  FuncExprBlockStmtPtr = std::shared_ptr<FuncExprBlockStmt>;

struct IfExpr : Expr {
    ExprPtr              condition;
    FuncExprBlockStmtPtr if_branch;
    FuncExprBlockStmtPtr else_branch;

    template<typename T1, typename T2, typename T3>
    IfExpr(T1&& condition, T2&& if_branch, T3&& else_branch)
        : condition(std::forward<T1>(condition)), if_branch(std::forward<T2>(if_branch)),
          else_branch(std::forward<T3>(else_branch)) {}

    constexpr NodeType type() const override { return NodeType::IfExpr; }
};
using  IfExprPtr = std::shared_ptr<IfExpr>;

void ast2Dot(std::ofstream& out, const ProgPtr p_prog);

} // namespace parser::ast
