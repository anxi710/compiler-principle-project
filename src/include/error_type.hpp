#pragma once

#include <cstdint>

namespace error
{

// 错误类型
enum class ErrorType : std::uint8_t
{
    Lex,      // 词法错误
    Parse,    // 语法错误
    Internal  // 内部代码错误
};

// 词法错误码
enum class LexErrorType : std::uint8_t
{
    UnknownToken,  // 未知的 token
};

// 语法错误码
enum class ParseErrorType : std::uint8_t
{
    UnexpectToken,  // 并非期望 token
};

// 编译器内部错误码
enum class InternalErrorType : std::uint8_t
{
    FileAccess,  // 文件访问

    UnknownCmdArg,  // 未知命令行参数
    MissingCmdArg,  // 缺失命令行参数

    UnknownKeyword,  // 未知关键字
};

}  // namespace error
