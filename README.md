# Compiler Principle Project

> 同济大学 2024-2025 学年春季学期 编译原理 课程作业

本项目使用 C++ 实现一个类 Rust 语言的词法和语法分析器。

## 代码风格约定

1. 尽量编写 `pure function`，而不是 `non-pure function`，即函数尽量不要有副作用 (`side effects`)
2. 所有动态申请的资源使用 `std::unique_ptr<>` or `std::shared_ptr<>` 管理
3. 使用新版 `C++` 中具有更明确语义的语法或库函数
   - 如表示不一定有返回值，则使用 `std::optional<>`；表示可能有错误需要返回，则使用 `std::expected<>`
   - 对于可迭代对象，使用迭代器或者范围 `for` 遍历
   - 使用初始化列表进行初始化，统一接口
4. 接口和实现分离，即头文件中只给出 function signature，在源文件中实现
   - 内联函数在头文件中定义
5. 使用 `namespace` 管理命名空间,避免命名污染
6. 命名规则
   - 函数名：小驼峰命名法
   - 类型名：大驼峰命名法
   - 文件名和变量名：蛇形命名法
7. 尽量编写单入口单出口函数
8. 完善的测试用例

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
│   ├── include                # 头文件
│   │   ├── ast.hpp            # 抽象语法树
│   │   ├── error_reporter.hpp # 错误报告器
│   │   ├── error_type.hpp     # 错误类型编码
│   │   ├── keyword_table.hpp  # 关键字表
│   │   ├── lexer.hpp          # 词法分析器
│   │   ├── lexer_position.hpp
│   │   ├── parser.hpp         # 语法分析器
│   │   ├── preproc.hpp        # 预处理相关工具函数
│   │   ├── token.hpp          # 词法单元
│   │   ├── token_type.hpp     # 词法单元种类
│   │   └── toy_lexer.hpp      # 实际的词法分析器（派生自 lexer）
│   ├── ast.cpp
│   ├── error_reporter.cpp
│   ├── main.cpp               # 程序入口点
│   ├── parser.cpp
│   ├── preproc.cpp
│   ├── token.cpp
│   ├── token_type.cpp
│   └── toy_lexer.cpp
└── test
    │── build      # 目标文件目录
    ├── Makefile   # 测试用例构建文件
    ├── test_case  # 用于词法和语法分析器的测试用例（一组用类 Rust 词法和语法编写的程序）
    └── *.cpp      # 模块测试用例
```
