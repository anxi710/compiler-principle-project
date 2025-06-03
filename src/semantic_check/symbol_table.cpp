#include "symbol_table.hpp"

#include <sstream>
#include <stdexcept>

namespace symbol
{

/**
 * @brief 进入作用域
 * @param name 作用域限定符
 */
void SymbolTable::enterScope(const std::string& name)
{
    if (scopes.contains(name))
    {
        throw std::runtime_error{"scope already exists"};
    }

    std::ostringstream oss;
    oss << cscope_name << "::" << name;
    cscope_name = oss.str();
    p_cscope = std::make_shared<Scope>();
    scopes[cscope_name] = p_cscope;
}

/**
 * @brief 退出作用域
 */
void SymbolTable::exitScope()
{
    std::size_t idx = cscope_name.find_last_of(':');
    if (idx >= cscope_name.length())
    {
        throw std::runtime_error{"can't exit scope"};
    }

    cscope_name = cscope_name.substr(0, idx - 1);
    if (!scopes.contains(cscope_name))
    {
        throw std::runtime_error{"can't find the upper-level scope"};
    }
    p_cscope = scopes[cscope_name];
}

/**
 * @brief 声明函数
 * @param fname  函数名
 * @param p_func 函数符号指针
 */
void SymbolTable::declareFunc(const std::string& fname, FunctionPtr p_func)
{
    if (funcs.contains(fname))
    {
        throw std::runtime_error{"function name already exists"};
    }

    funcs[fname] = std::move(p_func);
}

/**
 * @brief 声明变量
 * @param vname 变量名
 * @param p_var 变量符号指针
 */
void SymbolTable::declareVar(const std::string& vname, VariablePtr p_var)
{
    if (p_cscope->contains(vname))
    {
        throw std::runtime_error{"variable name already exists"};
    }

    (*p_cscope)[vname] = std::move(p_var);
}

/**
 * @brief  查找函数符号
 * @param  name 函数名
 * @return std::optional<FunctionPtr> 需要检查是否能查到
 */
[[nodiscard]]
auto SymbolTable::lookupFunc(const std::string& name) const -> std::optional<FunctionPtr>
{
    if (funcs.contains(name))
    {
        return funcs.find(name)->second;
    }
    return std::nullopt;
}

/**
 * @brief  查找变量符号
 * @param  name 变量名
 * @return std::optional<VariablePtr> 需要检查是否能查到
 */
[[nodiscard]]
auto SymbolTable::lookupVar(const std::string& name) const -> std::optional<VariablePtr>
{
    auto exit_scope = [](std::string& scope_name) -> bool
    {
        std::size_t idx = scope_name.find_last_of(':');
        if (idx >= scope_name.length())
        {
            return false;
        }
        scope_name = scope_name.substr(0, idx - 1);
        return true;
    };

    auto scope_name = cscope_name;

    bool flag = false;
    VariablePtr p_var;
    do
    {
        auto p_scope = scopes.find(scope_name)->second;
        if (p_scope->contains(name))
        {
            flag = true;
            p_var = p_scope->find(name)->second;
            break;
        }
    } while (exit_scope(scope_name));

    return flag ? std::optional<VariablePtr>{p_var} : std::nullopt;
}

}  // namespace symbol
