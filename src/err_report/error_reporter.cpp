#include "error_reporter.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

namespace error
{

// 控制字符
static inline const std::string RESET = "\033[0m";
static inline const std::string RED = "\033[1;31m";
static inline const std::string YELLOW = "\033[1;33m";
static inline const std::string BLUE = "\033[1;34m";
static inline const std::string BOLD = "\033[1m";

/*---------------- LexError ----------------*/

void ErrorReporter::displayUnknownType(const LexError& err) const
{
    std::cerr << BOLD << YELLOW << "warning[UnknownToken]" << RESET << BOLD
              << ": 识别到未知 token '" << err.token << "'" << RESET << std::endl;

    std::cerr << BLUE << " --> " << RESET << "<row: " << err.row + 1 << ", col: " << err.col + 1
              << ">" << std::endl;

    std::cerr << BLUE << "  |  " << std::endl
              << BLUE << " " << err.row + 1 << " | " << RESET << this->text[err.row] << std::endl;

    std::ostringstream oss;
    oss << " " << err.row + 1 << " | ";
    int delta = oss.str().length() + err.col - 3;
    std::cerr << BLUE << "  |" << std::string(delta, ' ') << "^" << RESET << std::endl << std::endl;
}

void ErrorReporter::displayLexErr(const LexError& err) const
{
    switch (err.type)
    {
        default:
            break;
        case LexErrorType::UnknownToken:
            displayUnknownType(err);
            break;
    }
}

/*---------------- LexError ----------------*/

/*---------------- ParseError ----------------*/

/*---------------- ParseError ----------------*/

/*---------------- SemanticError ----------------*/

static auto displaySemanticErrorType(SemanticErrorType type) -> std::pair<std::string, std::string>
{
    switch (type)
    {
        case SemanticErrorType::ArgCountMismatch:
            return {std::string{"ArgMismatch"}, std::string{"函数参数个数不匹配"}};
        case SemanticErrorType::VoidFuncReturnValue:
        case SemanticErrorType::FuncReturnTypeMismatch:
            return {std::string{"FuncReturnMismatch"}, std::string{"函数返回值类型不匹配"}};
        case SemanticErrorType::MissingReturnValue:
            return {std::string{"MissingReturnValue"}, std::string{"函数有返回值但未返回任何值"}};
        case SemanticErrorType::UndefinedFunctionCall:
            return {std::string{"UndefinedFunction"}, std::string{"函数未定义"}};
        case SemanticErrorType::UndeclaredVariable:
            return {std::string{"UndeclaredVariable"}, std::string{"变量未声明"}};
        case SemanticErrorType::UninitializedVariable:
            return {std::string{"UninitializedVariable"}, std::string{"变量未初始化"}};
        case SemanticErrorType::AssignToNonVariable:
            return {std::string{"InvalidAssignment"}, std::string{"无效赋值语句"}};
        case SemanticErrorType::AssignToUndeclaredVar:
            return {std::string{"InvalidAssignment"}, std::string{"无效赋值语句"}};
        case SemanticErrorType::TypeInferenceFailure:
            return {std::string{"TypeInferenceFailure"},
                    std::string{"变量无法通过自动类型推导确定类型"}};
        case SemanticErrorType::TypeMismatch:
            return {std::string{"TypeMismatch"}, std::string{"变量类型不匹配"}};
    }
}

void ErrorReporter::displaySemanticErr(const SemanticError& err) const
{
    auto pair = displaySemanticErrorType(err.type);
    std::cerr << BOLD << RED << "Error[" << pair.first << "]" << RESET << BOLD << ": "
              << pair.second << RESET << std::endl;

    std::cerr << BLUE << "--> " << RESET << "scope: " << err.scope_name << " <row: " << err.row + 1
              << ", col: " << err.col + 1 << ">" << std::endl;

    std::cerr << BLUE << "  |  " << std::endl
              << BLUE << " " << err.row + 1 << " | " << RESET << this->text[err.row] << std::endl;

    std::ostringstream oss;
    oss << " " << err.row + 1 << " | ";
    int delta = oss.str().length() + err.col - 3;
    std::cerr << BLUE << "  |" << std::string(delta, ' ') << "^" << RESET << std::endl << std::endl;

    std::cerr << "    Details: " << err.msg << std::endl << std::endl;
}

/*---------------- SemanticError ----------------*/

/*---------------- InternalError ----------------*/

/*---------------- InternalError ----------------*/

/*---------------- ErrorReporter ----------------*/

ErrorReporter::ErrorReporter(const std::string& t)
{
    std::istringstream iss{t};

    std::string line{};
    while (std::getline(iss, line))
    {
        this->text.push_back(line);
    }

    assert(iss.eof());
}

/**
 * @brief 报告词法错误
 * @param type      词法错误类型
 * @param msg       错误信息
 * @param r         错误发生的行数
 * @param c         错误发生的列数
 * @param token     错误发生的 token
 * @param terminate 是否终止
 */
void ErrorReporter::report(LexErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                           const std::string& token, bool terminate)
{
    lex_errs.emplace_back(type, msg, r, c, token);

    if (terminate)
    {
    }
}

void ErrorReporter::report(const LexError& le, bool terminate)
{
    lex_errs.push_back(le);

    if (terminate)
    {
    }
}

/**
 * @brief 报告语法错误
 * @param type      词法错误类型
 * @param msg       错误信息
 * @param r         错误发生的行数
 * @param c         错误发生的列数
 * @param token     错误发生的 token
 * @param terminate 是否终止
 */
void ErrorReporter::report(ParseErrorType type, const std::string& msg, std::size_t r,
                           std::size_t c, const std::string& token)
{
    parse_errs.emplace_back(type, msg, r, c, token);
}

/**
 * @brief 报告语义错误
 * @param type
 * @param msg
 * @param scope_name
 * @param terminate
 */
void ErrorReporter::report(SemanticErrorType type, const std::string& msg, std::size_t r,
                           std::size_t c, const std::string& scope_name)
{
    semantic_errs.emplace_back(type, msg, r, c, scope_name);
}

void ErrorReporter::displayLexErrs() const
{
    for (const auto& lex_err : lex_errs)
    {
        displayLexErr(lex_err);
    }
}

void ErrorReporter::displaySemanticErrs() const
{
    for (const auto& semantic_err : semantic_errs)
    {
        displaySemanticErr(semantic_err);
    }
}

}  // namespace error
