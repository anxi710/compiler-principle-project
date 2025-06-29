#include "symbol_table.hpp"

#include <cassert>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace symbol
{

/**
 * @brief 进入作用域
 * @param name         作用域限定符
 * @param create_scope 是否新建作用域
 */
void SymbolTable::enterScope(const std::string& name, bool create_scope)
{
    std::ostringstream oss;
    oss << cscope_name << "::" << name;

    if (!create_scope)
    {
        assert(scopes.contains(oss.str()));
        cscope_name = oss.str();
        p_cscope = scopes[cscope_name];
        return;
    }

    if (scopes.contains(name))
    {
        throw std::runtime_error{"scope already exists"};
    }

    cscope_name = oss.str();
    p_cscope = std::make_shared<Scope>();
    scopes[cscope_name] = p_cscope;
}

/**
 * @brief  退出作用域
 * @return 退出的作用域名
 */
auto SymbolTable::exitScope() -> std::string
{
    std::size_t idx = cscope_name.find_last_of(':');
    if (idx >= cscope_name.length())
    {
        throw std::runtime_error{"can't exit scope"};
    }

    std::string name = cscope_name.substr(idx + 1);
    cscope_name = cscope_name.substr(0, idx - 1);
    if (!scopes.contains(cscope_name))
    {
        throw std::runtime_error{"can't find the upper-level scope"};
    }
    p_cscope = scopes[cscope_name];

    return name;
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
    // if (p_cscope->contains(vname))
    // {
    //     throw std::runtime_error{"variable name already exists"};
    // }

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

/**
 * @brief  将变量类型枚举转换为对应的字符串表示
 * @param  vt 变量类型枚举值
 * @return 变量类型对应的字符串
 */
static auto varType2Str(VarType vt) -> std::string
{
    switch (vt)
    {
        case VarType::Null:
            return std::string{"Null"};
        // case VarType::Bool:
        //     return std::string{"Bool"};
        case VarType::I32:
            return std::string{"I32"};
        case VarType::Unknown:
            return std::string{"Unknown"};
    }

    throw std::runtime_error{"varType2Str(): error return."};
}

/**
 * @brief 打印符号表
 * @param out 输出流
 */
void SymbolTable::printSymbol(std::ofstream& out)
{
    out << "搜集到如下函数符号：" << std::endl;

    for (const auto& func : funcs)
    {
        out << "函数名：" << func.first << "，参数个数：" << func.second->argc << "，返回值类型："
            << varType2Str(func.second->retval_type) << std::endl;
    }

    out << "搜集到如下变量符号：" << std::endl;

    for (const auto& scope : scopes)
    {
        auto scope_name = scope.first;
        auto p_scope = scope.second;

        for (const auto& var : *p_scope)
        {
            out << "变量名：" << scope_name << "::" << var.first << "，类型："
                << varType2Str(var.second->var_type) << std::endl;
        }
    }
}

/**
 * @brief 取作用域名
 * @return 作用域名，含global
 */
auto SymbolTable::getCurScope() const -> const std::string&
{
    return cscope_name;
}

/**
 * @brief 取变量名
 * @return 变量全名，含作用域
 */
auto SymbolTable::getTempValName() -> std::string
{
    return std::format("t{}", tv_cnt++);
}

/**
 * @brief 取函数名
 * @return 函数名，不含global
 */
auto SymbolTable::getFuncName() -> std::string
{
    std::regex re{R"(^global::(\w+))"};
    std::smatch match;
    std::regex_search(cscope_name, match, re);
    return match[1];
}

/**
 * @brief 检查作用域下变量是否有类型
 * @return 未定义类型变量
 */
auto SymbolTable::checkAutoTypeInference() const -> std::vector<VariablePtr>
{
    std::vector<VariablePtr> failed_vars;

    for (const auto& [name, p_var] : *p_cscope)
    {
        if (p_var->var_type == VarType::Unknown)
        {
            failed_vars.push_back(p_var);
        }
    }

    return failed_vars;
}

}  // namespace symbol
