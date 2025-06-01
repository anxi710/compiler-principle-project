#include "symbol_table.hpp"

namespace symbol
{

auto SymbolTable::find(const std::string& n) -> SymbolPtr
{
    if (func_table.contains(n))
    {
        return func_table.find(n)->second;
    }
    if (var_table.contains(n))
    {
        return var_table.find(n)->second;
    }
    throw std::runtime_error{n + " is not a symobl"};
}

auto SymbolTable::inTable(const std::string& n) -> bool
{
    return !(func_table.contains(n) || var_table.contains(n));
}

void SymbolTable::addFunc(std::string n, std::vector<std::pair<VarType, RefType>> argv, VarType rvt)
{
    func_table[n] = std::make_shared<Function>(n, argv, rvt);
}

void SymbolTable::addInteger(std::string n, bool mut, RefType rt, std::optional<std::int32_t> val)
{
    var_table[n] = std::make_shared<Integer>(n, mut, rt, val);
}

}  // namespace symbol
