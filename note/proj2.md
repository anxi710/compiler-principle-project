# 大作业 2

## 需要实现的产生式

**0.1 变量声明内部**:

- VarDeclBody -> _mut_ \<ID\>

**0.2 类型**:

- VarType -> _i32_

**0.3 可赋值元素**:

- AssignElement -> Variable
- Variable -> \<ID\>

**1.1 基础规则**:

- Prog -> (Decl)*
- Decl -> FuncDecl
- FuncDecl -> FuncHeaderDecl BlockStmt
- FuncHeaderDecl -> _fn_ \<ID\> _(_ Args _)_
- Args -> $\epsilon$
- BlockStmt -> _{_ Stmts _}_
- Stmts -> $\epsilon$

**1.2 语句（前置规则 1.1）**:

- Stmts -> (Stmt)*
- Stmt -> _;_

**1.3 返回语句（前置规则 1.2）**:

- Stmt -> RetStmt
- RetStmt -> _return_ _;_

**1.4 函数输入（前置规则 0.1, 0.2, 1.1）**:

- Args -> Arg (_,_ Arg)*
- Arg -> VarDeclBody _:_ VarType

**1.5 函数输出（前置规则 0.2, 1.1, 1.3, 3.1）**:

- FuncHeaderDecl -> _fn_ \<ID\> _(_ Args _)_ _->_ VarType
- RetStmt -> _return_ Expr _;_

**2.1 变量声明语句（前置规则 0.1, 0.2, 1.2）**:

- Stmt -> VarDeclStmt
- VarDeclStmt -> _let_ VarDeclBody (_:_ VarType)? _;_

**2.2 赋值语句（前置规则 0.3, 1.2, 3.1）**:

- Stmt -> AssignStmt
- AssignStmt -> AssignElement _=_ Expr _;_

**3.1 基本表达式（前置规则 0.3）**:

- Stmt -> ExprStmt
- ExprStmt -> Expr _;_
- Expr -> ComparExpr
- ComparExpr -> ArithExpr
- ArithExpr -> Factor
- Factor -> Element
- Element -> \<NUM\> | AssignElement | ParenthesisExpr
- ParenthesisExpr -> _(_ Expr _)_
  - 现在的 \<NUM\> 只有整数 \<INT\>

**3.2 表达式增加计算和比较（前置规则 3.1）**:

- ComparExpr -> ArithExpr ([\< | \<= | \> | \>= | == | !=] ArithExpr)*
- ArithExpr -> Factor ([+ | - | \* | /] Factor)*

**3.3 函数调用（前置规则 3.1）**:

- Element -> \<ID\> _(_ Args _)_
- Args -> Expr (_,_ Expr)* | $\epsilon$

**4.1 选择结构（前置规则 1.2, 3.1）**:

- Stmt -> IfStmt
- IfStmt -> _if_ Expr BlockStmt ElseClause
- ElseClause -> _else_ BlockStmt | $\epsilon$

**5.1 while 循环结构（前置规则 1.2, 3.1）**:

- Stmt -> RepetitionStmt
- RepetitionStmt -> WhileStmt
- WhileStmt -> _while_ Expr BlockStmt

**综合如下**:

- Prog -> (FuncDecl)*
- FuncDecl -> FuncHeaderDecl BlockStmt
- FuncHeaderDecl -> "fn" "\<ID\>" "(" (Arg ("," Arg)*)? ")" ("->" VarType)?
- Arg -> VarDeclBody ":" VarType
- VarDeclBody -> "mut" "\<ID\>"
- VarType -> Integer
- Integer -> "i32"
- BlockStmt -> "{" (Stmt)* "}"
- Stmt -> VarDeclStmt | RetStmt | ExprStmt | AssignStmt | IfStmt | WhileStmt | NullStmt
- ExprStmt -> CallExpr ";" | CmpExpr ";"
- VarDeclStmt -> "let" "mut" "\<ID\>" (":" VarType)? ";"
- RetStmt -> "return" (CmpExpr)? ";"
- CallExpr -> "\<ID\>" "(" (Arg ("," Arg)*)? ")"
- AssignStmt -> AssignElement "=" Expr ";"
- AssignElement -> Variable
- Variable -> "\<ID\>"
- IfStmt -> "if" CmpExpr BlockStmt (ElseClause)*
- ElseClause -> "else" BlockStmt
- WhileStmt -> "while" CmpExpr BlockStmt
- NullStmt -> ";"
- CmpExpr -> AddExpr ([\< | \<= | \> | \>= | == | !=] AddExpr)*
- AddExpr -> MulExpr ([+ | -] MulExpr)*
- MulExpr -> Factor ([\* | /] Factor)*
- Factor -> Element | ParenthesisExpr
- Element -> ParenthesisExpr | "\<INT\>" | AssignElement | CallExpr | Variable
- ParenthesisExpr -> "(" CmpExpr ")"

## 语义规则

- 0.1: 声明变量为可变变量，允许二次赋值
- 0.2: 支持基础类型 i32，表示 32 bit 整数
- 0.3: 标识符可以作为左值，被用于进行赋值操作
- 1.1: 最基础的程序结构支持声明若干个函数，每个函数无参数且函数体为空
- 1.2: 函数体内允许有任意个语句，单个分号可以作为一个语句
- 1.3: 返回语句用于从函数调用中退出，并将控制权交还给调用者。当前仅支持不返回任何结果的返回语句
- 1.4: 允许函数声明时接收参数，参数具有指定的类型信息
- 1.5: 函数头声明支持标注返回类型，并将返回语句增强为支持返回一个表达式的值
- 2.1: 支持变量声明语句，声明时允许不指定类型。若指定了类型，则必须与后续赋值或使用的表达式类型一致；若未指定类型，则需要根据后续使用的上下文进行类型推导（若无法推导，则报错）
  - 变量允许二次声明，每次声明时隐藏之前的绑定（称为重影）
- 2.2: 允许将一个表达式的计算结果赋值给某个左值，该左值需要提前声明
- 3.1: 支持常量、变量以及带括号的嵌套表达式
- 3.2: 支持比较、加减、乘除三种运算，且优先级乘除 > 加减 > 比较
- 3.3: 支持函数调用表达式，实参与形参的数量及类型需一致；函数若无返回值，则不能出现在需要右值的上下文中
- 4.1: 支持 if 语句及可选的 else 分支
- 5.1: 支持 while 语句，每次循环开始前求表达式的值

_补充_：

- 变量作为右值求值时，需要检查变量是否初始化
- 不允许声明同名函数
