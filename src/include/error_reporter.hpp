#pragma once

#include <list>
#include <vector>
#include <string>
#include <memory>
#include <source_location>
#include "error_type.hpp"


namespace error {

struct Error {
    std::string msg; // error message

    Error() = default;
    Error(const std::string& m) : msg(m) {}
    Error(std::string&& m) : msg(std::move(m)) {}

    virtual constexpr ErrorType kind() const = 0;
    virtual ~Error() = default;
};

struct LexError : Error {
    LexErrorType type;
    std::size_t  row;
    std::size_t  col;
    std::string  token;

    LexError() = delete;

    explicit LexError(LexErrorType type, const std::string& msg,
        std::size_t r, std::size_t c, const std::string& token)
        : Error(msg), type(type), row(r), col(c), token(token) {}

    ~LexError() override = default;

    constexpr ErrorType kind() const override { return ErrorType::Lex; }
};

struct ParseError : Error {
    ParseErrorType type;
    std::size_t    row;
    std::size_t    col;
    std::string    token;

    ParseError() = delete;

    explicit ParseError(ParseErrorType type, const std::string& msg,
        std::size_t r, std::size_t c, const std::string& token)
        : Error(msg), type(type), row(r), col(c), token(token) {}

    ~ParseError() override = default;

    constexpr ErrorType kind() const override { return ErrorType::Parse; }
};

struct InternalError : Error {
    InternalErrorType    type;
    std::source_location location;

    InternalError() = delete;

    template<typename T>
    explicit InternalError(InternalErrorType t, T&& msg, const std::source_location loc)
        : Error(std::forward<T>(msg)), type(t), location(loc) {}

    constexpr ErrorType kind() const override { return ErrorType::Internal; }
};

class ErrorReporter {
public:
    ErrorReporter() = delete;
    explicit ErrorReporter(const std::string& t);

    ~ErrorReporter() = default;

public:
    void report(LexErrorType type, const std::string& msg,
        std::size_t r, std::size_t c, const std::string& token, bool terminate = false);
    void report(LexError le, bool terminate = false);

    void report(ParseErrorType type, const std::string& msg,
        std::size_t r, std::size_t c, const std::string& token, bool terminate = false);

    [[noreturn]] // 编译器内部错误必须终止
    void report(InternalErrorType t, const std::string& msg,
        const std::source_location loc = std::source_location::current());

    [[noreturn]]
    void terminateProg();

public:
    void displayLexErrs() const;
    void displayParseErrs() const;
    void displayInternalErrs() const;

    inline bool hasLexErr() const { return !lex_errs.empty(); }
    inline bool hasParseErr() const { return !parse_errs.empty(); }

private:
    inline bool hasErrs() const {
        return !(lex_errs.empty() && parse_errs.empty());
    }

    void displayLexErr(const LexError& err) const;
    void displayUnknownType(const LexError& err) const;

private:
    std::vector<std::string> text;          // 输入文件原始文本
    std::list<LexError>      lex_errs;      // 词法错误列表
    std::list<ParseError>    parse_errs;    // 语法错误列表
};

} // namespace error
