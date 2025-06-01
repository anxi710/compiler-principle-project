#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
    Integer,
    Array,
    Tuple
};

enum class RefType : std::uint8_t
{
    Normal,    // 普通变量
    Mutable,   // 可变引用
    Immutable  // 不可变引用
};

struct Variable : Symbol
{
    bool mut = true;                      // 变量本身是否可变
    RefType ref_type = RefType::Normal;   // 若为引用变量，是否允许改变引用对象
    VarType var_type = VarType::Integer;  // 变量类型

    Variable() = default;
    explicit Variable(const std::string& n, const RefType& rt, const VarType& vt)
        : Symbol(n), ref_type(rt), var_type(vt)
    {
    }
    ~Variable() override = default;
};
using VariablePtr = std::shared_ptr<Variable>;

struct Integer
{
    std::optional<std::int32_t> init_value;  // 初始值
};

struct Function : Symbol
{
    std::vector<std::pair<VarType, RefType>> argv;
    VarType retval_type;

    Function() : argv({}), retval_type(VarType::Integer) {}
    template <typename T>
    Function(std::string name, T&& argv, VarType retval_type)
        : Symbol(name), argv(std::forward<T>(argv)), retval_type(retval_type)
    {
    }
};
using FunctionPtr = std::shared_ptr<Function>;

class SymbolTable
{
   public:
    SymbolTable() = default;
    ~SymbolTable();

   public:
    auto find(std::string name) -> SymbolPtr;
    // auto addFunc(const std::vector<std::pair<VarType, RefType>> argv)

   private:
    std::unordered_map<std::string, FunctionPtr> func_table;  // 函数符号表
    std::unordered_map<std::string, VariablePtr> var_table;   // 变量符号表
};

}  // namespace symbol
