#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace symbol
{

struct Symbol
{
    std::string name;

    Symbol() = default;
    explicit Symbol(std::string n) : name(std::move(n)) {}
    virtual ~Symbol() = default;
};
using SymbolPtr = std::shared_ptr<Symbol>;

enum class VarType : std::uint8_t
{
    Null,  // 返回值类型可以为空，即代表不返回任何值
    Bool,
    I32,
    // Array,
    // Tuple
};

// enum class RefType : std::uint8_t
// {
//     Normal,    // 普通变量
//     Mutable,   // 可变引用
//     Immutable  // 不可变引用
// };

struct Variable : Symbol
{
    // bool mut = true;                      // 变量本身是否可变
    // RefType ref_type = RefType::Normal;   // 若为引用变量，是否允许改变引用对象
    bool first_use = true;
    bool formal = false;
    VarType var_type = VarType::I32;  // 变量类型

    Variable() = default;
    explicit Variable(std::string n, bool formal, VarType vt)
        : Symbol(std::move(n)), formal(formal), var_type(vt)
    {
    }
    ~Variable() override = default;
};
using VariablePtr = std::shared_ptr<Variable>;

struct Integer : Variable
{
    std::optional<std::int32_t> init_val;  // 初值
    Integer() = default;
    Integer(std::string n, bool formal, std::optional<std::int32_t> val)
        : Variable(std::move(n), formal, VarType::I32), init_val(val)
    {
    }
    ~Integer() override = default;
};
using IntegerPtr = std::shared_ptr<Integer>;

struct Function : Symbol
{
    int argc;  // 参数个数 -- 基本规则中，不涉及到不可变参数及非 i32 类型变量，因此只需记录参数个数
    bool auto_deduce;
    VarType retval_type;

    Function() : argc(0), auto_deduce(false), retval_type(VarType::Null) {}
    Function(std::string n, int argc, bool ad, VarType rvt)
        : Symbol(std::move(n)), argc(argc), auto_deduce(ad), retval_type(rvt)
    {
    }

    void setRetvalType(VarType rvt) { retval_type = rvt; }
};
using FunctionPtr = std::shared_ptr<Function>;

class SymbolTable
{
   public:
    SymbolTable()
    {
        p_cscope = std::make_shared<Scope>();
        cscope_name = "global";
        scopes[cscope_name] = p_cscope;
    }
    ~SymbolTable() = default;

   public:
    void enterScope(const std::string& name);
    void exitScope();

    void declareFunc(const std::string& fname, FunctionPtr p_func);
    void declareVar(const std::string& vname, VariablePtr p_var);

    // 允许函数和变量同名，因此分成两部分处理
    [[nodiscard]]
    auto lookupFunc(const std::string& name) const -> std::optional<FunctionPtr>;

    [[nodiscard]]
    auto lookupVar(const std::string& name) const -> std::optional<VariablePtr>;

    void printSymbol(std::ofstream& out);

   private:
    using Scope = std::unordered_map<std::string, VariablePtr>;
    using ScopePtr = std::shared_ptr<Scope>;
    ScopePtr p_cscope;        // pointer (current scope)
    std::string cscope_name;  // 作用域限定符

    std::unordered_map<std::string, ScopePtr> scopes;
    std::unordered_map<std::string, FunctionPtr> funcs;
};

}  // namespace symbol
