#pragma once

namespace error {

// 错误类型
enum class ErrorType {
    Lex,     // 词法错误
    Parse,   // 语法错误
    Internal // 内部代码错误
};

// 词法错误码
enum class LexErrorType {
    UnknownToken, // 未知的 token
};

// 语法错误码
enum class ParseErrorType {
    UnexpectToken, // 并非期望 token
};

// 编译器内部错误码
enum class InternalErrorType {
    FileAccess,     // 文件访问

    UnknownCmdArg,  // 未知命令行参数
    MissingCmdArg,  // 缺失命令行参数

    UnknownKeyword, // 未知关键字
};

} // namespace error
