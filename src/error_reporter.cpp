#include <cassert>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "include/error_reporter.hpp"

namespace error {

// 控制字符
inline static const std::string RESET  = "\033[0m";
inline static const std::string RED    = "\033[1;31m";
inline static const std::string YELLOW = "\033[1;33m";
inline static const std::string BLUE   = "\033[1;34m";
inline static const std::string BOLD   = "\033[1m";

/*---------------- LexError ----------------*/

void ErrorReporter::displayUnknownType(const LexError& err) const {
    std::cerr << BOLD << YELLOW << "warning[UnknownToken]" << RESET << BOLD
        << ": 识别到未知 token '" << err.token << "'" << RESET << std::endl;

    std::cerr << BLUE << " --> " << RESET << "<row: " << err.row + 1 << ", col: " << err.col + 1 << ">" << std::endl;

    std::cerr << BLUE << "  |  " << std::endl
        << BLUE << " " << err.row + 1 << " | " << RESET << this->text[err.row] << std::endl;

    std::ostringstream oss;
    oss << " " << err.row + 1 << " | ";
    int delta = oss.str().length() + err.col - 3;
    std::cerr << BLUE << "  |" << std::string(delta, ' ') << "^" << RESET << std::endl << std::endl;
}

void ErrorReporter::displayLexErr(const LexError& err) const {
    switch(err.type) {
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

/*---------------- InternalError ----------------*/



/*---------------- InternalError ----------------*/


/*---------------- ErrorReporter ----------------*/

ErrorReporter::ErrorReporter(const std::string& t) {
    std::istringstream iss  {t};

    std::string        line {};
    while (std::getline(iss, line)) {
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
void ErrorReporter::report(LexErrorType type, const std::string& msg,
    std::size_t r, std::size_t c, const std::string& token, bool terminate) {
    lex_errs.emplace_back(type, msg, r, c, token);

    if (terminate) {
        terminateProg();
    }

    return;
}

void ErrorReporter::report(LexError le, bool terminate) {
    this->lex_errs.push_back(le);

    if (terminate) {
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
void ErrorReporter::report(ParseErrorType type, const std::string& msg,
    std::size_t r, std::size_t c, const std::string& token, bool terminate) {
    parse_errs.emplace_back(type, msg, r, c, token);

    if (terminate) {
        terminateProg();
    }

    return;
}

/**
 * @brief 报告编译器内部错误
 * @param t         词法错误类型
 * @param msg       错误信息
 * @param loc       错误发生位置
 */
void ErrorReporter::report(InternalErrorType t, const std::string& msg, const std::source_location loc) {
}

void ErrorReporter::displayLexErrs() const {
    for (const auto& lex_err : lex_errs) {
        displayLexErr(lex_err);
    }
}

void ErrorReporter::terminateProg() {
    if (hasErrs()) {
    }

    std::exit(1);
}

} // namespace error
