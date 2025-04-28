# Compiler Principle Project

> 同济大学 2024-2025 学年春季学期 编译原理 课程作业

本项目使用 C++ 实现一个类 Rust 语言的词法和语法分析器。

## 代码风格约定

1. 尽量编写 `pure function`，而不是 `non-pure function`，即函数尽量不要有副作用 (`side effects`)
2. 所有动态申请的资源使用 `std::unique_ptr<>` or `std::shared_ptr<>` 管理
3. 接口和实现分离，即头文件中只给出 function signature，在源文件中定义
   - 内联函数在头文件中定义
4. 使用 `namespace` 管理命名空间，避免命名污染
5. 命名规则
   - 函数名：小驼峰命名法
   - 类型名：大驼峰命名法
   - 文件名和变量名：蛇形命名法
6. 尽量编写单入口单出口函数
7. 错误处理健全
   - 善用 `assert`、`exception` 等
8. 编写一定的测试用例

## 文件组织结构说明

```shell
.
├── build            # 目标文件目录
├── docs             # 参考资料
│   ├── allstar.pdf  # Adaptive LL(*) Algorithm
│   ├── maximal_munch.pdf
│   └── 【Rust版】大作业1：词法和语法分析工具设计与实现.pdf
├── note           # 设计文档及笔记
│   └── parser.md  # 语法分析器相关设计文档
├── Makefile       # 构建文件
├── README.md      # this file
├── src            # 源代码
│   ├── ast.cpp
│   ├── include                # 头文件
│   │   ├── ast.hpp            # Abstract Syntax Tree
│   │   ├── keyword_table.hpp  # Keyword Table
│   │   ├── lexer.hpp          # Lexer
│   │   ├── parser.hpp         # Parser
│   │   ├── preproc.hpp        # Preprocess
│   │   ├── token.hpp          # Token
│   │   ├── token_type.hpp     # Token Type
│   │   ├── toy_lexer.hpp      # Toy Lexer
│   │   └── utils.hpp          # Utilities
│   ├── main.cpp
│   ├── parser.cpp
│   ├── preproc.cpp
│   ├── token.cpp
│   ├── token_type.cpp
│   ├── toy_lexer.cpp
│   └── utils.cpp
└── test
    │── build      # 目标文件目录
    ├── Makefile   # 测试用例构建文件
    ├── test_case  # 用于词法和语法分析器的测试用例（一组用类 Rust 词法和语法编写的程序）
    └── *.cpp      # 模块测试用例

```
