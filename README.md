# Compiler Principle Project

> 同济大学 2024-2025 学年春季学期 编译原理 课程作业

本项目使用 C++ 实现一个类 Rust 语言的词法和语法分析器。

## 代码风格约定

1. 尽量编写 `pure function`，而不是 `non-pure function`，即函数尽量不要有副作用 (`side effects`)
2. 所有动态申请的资源使用 `std::unique_ptr<>` or `std::shared_ptr<>` 管理
3. 接口和实现分离，即头文件中只给出 function signature，在源文件中定义
   - 内联函数在头文件中定义
4. 使用 `namespace` 管理命名空间，避免命名污染

## 文件组织结构说明

```shell
.
├── docs         # 文档
├── Makefile     # 构建文件
├── note         # 参考资料和笔记
├── README.md    # this file
└── src          # 源代码
    └── include  # 头文件
```
