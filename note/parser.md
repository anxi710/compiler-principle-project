# 语法分析器

## 类 Rust 语法规则

**0.1 变量声明内部**:

- VarDeclBody -> _mut_ \<ID\>

**0.2 类型**:

- VarType -> _i32_

**0.3 可赋值元素**:

- AssignElement -> Variable
- Variable -> \<ID\>

**1.1 基础规则**:

- Prog -> Decls
- Decls -> Decl Decls | $\epsilon$
  - Decls -> (Decl)*
- Decl -> FuncDecl
- FuncDecl -> FuncHeaderDecl BlockStmt
- FuncHeaderDecl -> _fn_ \<ID\> _(_ Args _)_
- Args -> $\epsilon$
- BlockStmt -> _{_ Stmts _}_
- Stmts -> $\epsilon$

**1.2 语句（前置规则 1.1）**:

- Stmts -> Stmt Stmts
- Stmt -> _;_

**1.3 返回语句（前置规则 1.2）**:

- Stmt -> RetStmt
- RetStmt -> _return_ _;_

**1.4 函数输入（前置规则 0.1, 0.2, 1.1）**

- Args -> Arg | Arg _,_ Arg        // 单个表达式没有逗号不是元组，而是普通括号表达式
                expect(TokenType::RPAREN, "Expected ')'");
                return firstExpr;s
  - 迭代形式：Args -> Arg (_,_ Arg)*
- Arg -> VarDeclBody _:_ VarType

**1.5 函数输出（前置规则 0.2, 1.1, 1.3, 3.1）**

- FuncHeaderDecl -> _fn_ \<ID\> _(_ Args _)_ _->_ VarType
- RetStmt -> _return_ Expr _;_

**2.1 变量声明语句（前置规则 0.1, 0.2, 1.2）**:

- Stmt -> VarDeclStmt
- VarDeclStmt -> _let_ VarDeclBody (_:_ VarType)? _;_

**2.2 赋值语句（前置规则 0.3, 1.2, 3.1）**:

- Stmt -> AssignStmt
- AssignStmt -> AssignElement _=_ Expr _;_

**2.3 变量声明赋值语句（前置规则 0.1, 0.2, 1.2, 3.1）**:

- Stmt -> VarDeclAssignStmt
- VarDeclAssignStmt -> _let_ VarDeclBody (_:_ VarType)? _=_ Expr _;_

**3.1 基本表达式（前置规则 0.3）**:

- Stmt -> Expr _;_
- Expr -> AddExpr
- AddExpr -> Item
- Item -> Factor
- Factor -> Element
- Element -> \<NUM\> | AssignElement | ParenthesisExpr
- ParenthesisExpr -> _(_ Expr _)_
  - 现在的 \<NUM\> 只有整数 \<INT\>

**3.2 表达式增加计算和比较（前置规则 3.1）**:

- Expr -> Expr [\< | \<= | \> | \>= | == | !=] AddExpr
- AddExpr -> AddExpr [+ | -] Item
- Item -> Item [\* | /] Factor

**3.3 函数调用（前置规则 3.1）**

- Element -> \<ID\> _(_ Args _)_
- Args -> Expr | Expr _,_ Args | $\epsilon$
  - Args -> Expr (_,_ Expr)* | $\epsilon$

**4.1 选择结构（前置规则 1.2, 3.1）**

- Stmt -> IfStmt
- IfStmt -> _if_ Expr BlockStmt ElseClause
- ElseClause -> _else_ BlockStmt | $\epsilon$

**4.2 增加 else if（前置规则 4.1）**

- ElseClause -> _else_ _if_ Expr BlockStmt ElseClause

**5.1 while 循环结构（前置规则 1.2, 3.1）**:

- Stmt -> RepetitionStmt
- RepetitionStmt -> WhileStmt
- WhileStmt -> _while_ Expr BlockStmt

**5.2 for 循环结构（前置规则 5.1）**:

- RepetitionStmt -> ForStmt
- ForStmt -> _for_ VarDecl _in_ IterativeStructure
- IterativeStructure -> Expr _.._ Expr

**5.3 loop 循环结构（前置规则 5.1）**:

- RepetitionStmt -> LoopStmt
- LoopStmt -> _loop_ BlockStmt

**5.4 增加 break 和 continue（前置规则 5.1）**:

- Stmt -> _break_ _;_ | _continue_ _;_

**6.1 声明不可变变量（前置规则 0.2）**:

- VarDecl -> \<ID\>

**6.2 借用和引用（前置规则 3.1, 6.1）**:

- Factor -> _&_ _mut_ Factor | _&_ Factor
- VarType -> _&_ mut VarType | _&_ VarType

`&mut` - 可变引用；`&` - 不可变引用；`*` - 解引用

- AssignElement -> *Variable

**7.1 函数表达式块（前置规则 1.2, 3.1）**:

- Expr -> FuncExprBlockStmt
- FuncExprBlockStmt -> _{_ FuncExprStmts _}_
- FuncExprStmts -> Expr | Stmt FuncExprStmts

**7.2 函数表达式块作为函数体**:

- FuncDecl -> FuncHeaderDecl FuncExprBlockStmt

**7.3 选择表达式（前置规则 7.1）**:

- Expr -> SelectExpr
- SelectExpr -> _if_ Expr FuncExprBlockStmt _else_ FuncExprBlockStmt

**7.4 循环表达式（前置规则 7.1）**:

- Expr -> LoopStmt
- Stmt -> _break_ Expr _;_

**8.1 数组（前置规则 0.2, 3.1）**:

- VarType -> _\[_ VarType _;_ \<INT\> _\]_
- Factor -> _\[_ ArrayElements _\]_
- ArrayElements -> $\epsilon$ | Expr | Expr _,_ ArrayElements
  - Factor 用于展开表达式

**8.2 数组元素（前置规则 8.1）**

- AssignElement -> Element _[_ Expr _]_
- IterativeStructure -> Element

**9.1 元组（前置规则 0.2, 3.1）**:

- VarType -> _(_ TupleType _)_
- TupleType -> VarType _,_ VarTypeList
- VarTypeList -> $\epsilon$ | VarType | VarType _,_ VarTypeList
- Factor -> _(_ TupleAssign _)_
- TupleAssign -> Expr _,_ TupleElements
- TupleElements -> $\epsilon$ | Expr | Expr _,_ TupleElements

**9.2 元组元素（前置规则 8.1）**:

- AssignElement -> Factor _._ \<INT\>
