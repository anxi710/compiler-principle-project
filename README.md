# Compiler Principle Project

> 同济大学 2024-2025 学年春季学期 编译原理 课程作业

本项目使用 C++ 实现一个类 Rust 语言的词法和语法分析器。

## 文件组织结构说明

```shell
.
├── build            # 目标文件目录
├── .clang-format    # clang-format 格式设置
├── .clang-tidy      # clang-tidy lint 检查设置
├── .editorconfig    # 基础格式设置
├── docs             # 参考资料
│   ├── allstar.pdf  # Adaptive LL(*) Algorithm
│   ├── maximal_munch.pdf
│   ├── 【Rust版】大作业1：词法和语法分析工具设计与实现.pdf
│   └── 【Rust版】大作业2：中间代码生成器.pdf
├── note           # 设计文档及笔记
│   ├── parser.md  # 语法分析器相关设计文档
│   └── proj2.md
├── Makefile       # 构建文件
├── README.md      # this file
├── report
├── src                # 源代码
│   ├── preproc        # 预处理
│   ├── lexer          # 词法分析
│   ├── parser         # 语法分析
│   ├── semantic_check # 语义检查
│   ├── ir_generate    # 中间代码生成
│   ├── err_report     # 错误报告
│   └── main.cpp       # 程序入口点
└── test
    │── build       # 目标文件目录
    ├── Makefile    # 测试用例构建文件
    ├── test_case   # 用于词法和语法分析器的测试用例（一组用类 Rust 词法和语法编写的程序）
    └── test_case_2 # 用于语义检查和中间代码生成器的测试用例
```

---

该项目还在等待重构中~
