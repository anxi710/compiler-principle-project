#pragma once

#include <list>
#include <string>
#include <vector>

#include "error_type.hpp"

namespace error
{

struct Error
{
    std::string msg;  // error message

    Error() = default;
    Error(const std::string& m) : msg(m) {}
    Error(std::string&& m) : msg(std::move(m)) {}

    [[nodiscard]]
    virtual constexpr auto kind() const -> ErrorType = 0;
    virtual ~Error() = default;
};

// 词法错误
struct LexError : Error
{
    LexErrorType type;
    std::size_t row;
    std::size_t col;
    std::string token;

    LexError() = delete;

    explicit LexError(LexErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                      std::string token)
        : Error(msg), type(type), row(r), col(c), token(std::move(token))
    {
    }

    ~LexError() override = default;

    [[nodiscard]]
    constexpr auto kind() const -> ErrorType override
    {
        return ErrorType::Lex;
    }
};

// 语法错误
struct ParseError : Error
{
    ParseErrorType type;
    std::size_t row;
    std::size_t col;
    std::string token;

    ParseError() = delete;

    explicit ParseError(ParseErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                        std::string token)
        : Error(msg), type(type), row(r), col(c), token(std::move(token))
    {
    }

    ~ParseError() override = default;

    [[nodiscard]]
    constexpr auto kind() const -> ErrorType override
    {
        return ErrorType::Parse;
    }
};

// 语义错误
struct SemanticError : Error
{
    SemanticErrorType type;
    std::size_t row;
    std::size_t col;
    std::string scope_name;

    SemanticError() = delete;

    explicit SemanticError(SemanticErrorType type, const std::string& msg, std::size_t r,
                           std::size_t c, std::string scope_name)
        : Error(msg), type(type), row(r), col(c), scope_name(std::move(scope_name))
    {
    }
    ~SemanticError() override = default;

    [[nodiscard]]
    constexpr auto kind() const -> ErrorType override
    {
        return ErrorType::Semantic;
    }
};

// 错误报告器
class ErrorReporter
{
   public:
    ErrorReporter() = delete;
    explicit ErrorReporter(const std::string& t);

    ~ErrorReporter() = default;

   public:
    void report(LexErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                const std::string& token, bool terminate = false);
    void report(const LexError& le, bool terminate = false);

    void report(ParseErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                const std::string& token);
    void report(SemanticErrorType type, const std::string& msg, std::size_t r, std::size_t c,
                const std::string& scope_name);

   public:
    void displayLexErrs() const;
    void displayParseErrs() const;
    void displayInternalErrs() const;
    void displaySemanticErrs() const;

    [[nodiscard]]
    auto hasLexErr() const -> bool
    {
        return !lex_errs.empty();
    }
    [[nodiscard]]
    auto hasParseErr() const -> bool
    {
        return !parse_errs.empty();
    }
    [[nodiscard]]
    auto hasSemanticErr() const -> bool
    {
        return !semantic_errs.empty();
    }

   private:
    [[nodiscard]]
    auto hasErrs() const -> bool
    {
        return !(lex_errs.empty() && parse_errs.empty() && parse_errs.empty());
    }

    void displayLexErr(const LexError& err) const;
    void displayUnknownType(const LexError& err) const;

    void displaySemanticErr(const SemanticError& err) const;

   private:
    std::vector<std::string> text;           // 输入文件原始文本
    std::list<LexError> lex_errs;            // 词法错误列表
    std::list<ParseError> parse_errs;        // 语法错误列表
    std::list<SemanticError> semantic_errs;  // 语义错误列表
};

}  // namespace error
