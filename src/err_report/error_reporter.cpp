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

void ErrorReporter::displayArgCountMismatch(const SemanticError& err) const
{
    std::cerr << BOLD << RED << "ERROR[ArgMismatch]" << RESET << BOLD << "函数参数个数不匹配"
              << RESET << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displayFuncReturnMismatch(const SemanticError& err) const
{
    std::cerr << BOLD << RED << "ERROR[FuncReturnMismatch]" << RESET << BOLD << "函数与返回值不匹配"
              << RESET << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displayUndefinedFunctionCall(const SemanticError& err) const
{
    std::cerr << BOLD << RED << "ERROR[UndefinedFunction]" << RESET << BOLD << "函数未定义" << RESET
              << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displayUndeclaredVariable(const SemanticError& err) const
{
    std::cerr << BOLD << RED << "ERROR[UndeclaredVariable]" << RESET << BOLD << "变量未声明"
              << RESET << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displayUninitializedVariable(const SemanticError& err) const

{
    std::cerr << BOLD << RED << "ERROR[UninitializedVariable]" << RESET << BOLD << "变量未初始化"
              << RESET << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displayInvalidAssignment(const SemanticError& err) const
{
    std::cerr << BOLD << RED << "ERROR[InvalidAssignment]" << RESET << BOLD << "无效赋值语句"
              << RESET << std::endl;

    std::cerr << BLUE << "-->" << RESET << "scope: " << err.scope_name << std::endl;
    std::cerr << "Details:" << err.msg << std::endl;
}

void ErrorReporter::displaySemanticErr(const SemanticError& err) const
{
    switch (err.type)
    {
        default:
            break;
        case error::SemanticErrorType::ArgCountMismatch:
            displayArgCountMismatch(err);
            break;
        case error::SemanticErrorType::FuncReturnTypeMismatch:
            displayFuncReturnMismatch(err);
            break;
        case error::SemanticErrorType::VoidFuncReturnValue:
            displayFuncReturnMismatch(err);
            break;
        case error::SemanticErrorType::MissingReturnValue:
            displayFuncReturnMismatch(err);
            break;
        case error::SemanticErrorType::UndefinedFunctionCall:
            displayUndefinedFunctionCall(err);
            break;
        case error::SemanticErrorType::UninitializedVariable:
            displayUninitializedVariable(err);
            break;
        case error::SemanticErrorType::UndeclaredVariable:
            displayUndeclaredVariable(err);
            break;
        case error::SemanticErrorType::AssignToUndeclaredVar:
            displayInvalidAssignment(err);
            break;
        case error::SemanticErrorType::AssignToNonVariable:
            displayInvalidAssignment(err);
            break;
    }
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
        terminateProg();
    }
}

void ErrorReporter::report(const LexError& le, bool terminate)
{
    this->lex_errs.push_back(le);

    if (terminate)
    {
        terminateProg();
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
                           std::size_t c, const std::string& token, bool terminate)
{
    parse_errs.emplace_back(type, msg, r, c, token);

    if (terminate)
    {
        terminateProg();
    }
}

/**
 * @brief 报告语义错误
 * @param type
 * @param msg
 * @param scope_name
 * @param terminate
 */
void ErrorReporter::report(SemanticErrorType type, const std::string& msg,
                           const std::string& scope_name, bool terminate)
{
    semantic_errs.emplace_back(type, msg, scope_name);

    if (terminate)
    {
        terminateProg();
    }
}

/**
 * @brief 报告编译器内部错误
 * @param t         词法错误类型
 * @param msg       错误信息
 * @param loc       错误发生位置
 */
void ErrorReporter::report(InternalErrorType t, const std::string& msg,
                           const std::source_location& loc)
{
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

void ErrorReporter::terminateProg()
{
    if (hasErrs())
    {
    }

    std::exit(1);
}

}  // namespace error
