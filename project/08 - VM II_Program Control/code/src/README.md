# VM Translator (Nand to Tetris Project 8)

这是一个完整的 VM 翻译器，支持 Hack VM 语言的所有功能，包括程序流程控制和函数调用。

## 项目结构

```
src/
├── VMTranslator.cpp   # 主程序（支持单文件和目录翻译）
├── Parser.h           # VM 命令解析器
├── CodeWriter.h       # 汇编代码生成器（完整功能）
├── Makefile           # 编译和测试配置
└── VMTranslator       # 编译后的可执行文件
```

## 功能特性

### 支持的 VM 命令

#### 1. 算术/逻辑命令（Project 7）
- `add`, `sub`, `neg`
- `eq`, `gt`, `lt`
- `and`, `or`, `not`

#### 2. 内存访问命令（Project 7）
- `push segment index`
- `pop segment index`
- 支持的段：constant, local, argument, this, that, temp, pointer, static

#### 3. 程序流程控制（Project 8 新增）
- `label label` - 声明标签
- `goto label` - 无条件跳转
- `if-goto label` - 条件跳转

#### 4. 函数调用（Project 8 新增）
- `function functionName nVars` - 函数声明
- `call functionName nArgs` - 函数调用
- `return` - 函数返回

## 编译

在 `src` 目录下运行：

```bash
make
```

## 使用方法

### 翻译单个 VM 文件（不生成启动代码）

用于基础测试（无函数调用）：

```bash
./VMTranslator <file.vm>
```

例如：
```bash
./VMTranslator ../test/BasicLoop/BasicLoop.vm
./VMTranslator ../test/SimpleFunction/SimpleFunction.vm
```

### 翻译目录（生成启动代码）

用于包含函数调用的完整程序：

```bash
./VMTranslator <directory>
```

例如：
```bash
./VMTranslator ../test/FibonacciElement
./VMTranslator ../test/StaticsTest
```

**重要规则**：
- 翻译**单个文件**时：**不生成**启动代码
- 翻译**目录**时：**生成**启动代码（初始化 SP=256 并调用 Sys.init）

## 测试

### 测试程序流程控制

```bash
make test-flow
```

测试文件：
- BasicLoop.vm
- FibonacciSeries.vm

### 测试函数调用（单文件）

```bash
make test-simple-function
```

### 测试函数调用（目录）

```bash
make test-fibonacci    # FibonacciElement
make test-statics      # StaticsTest
make test-nested       # NestedCall
```

### 运行所有测试

```bash
make test
```

## 实现细节

### 启动代码（Bootstrap Code）

当翻译目录时，生成以下启动代码：

```asm
@256
D=A
@SP
M=D        // SP = 256
// call Sys.init 0
```

### 函数调用约定

#### Call 操作
1. 保存返回地址
2. 保存调用者的 LCL, ARG, THIS, THAT
3. 设置被调用函数的 ARG = SP - 5 - nArgs
4. 设置被调用函数的 LCL = SP
5. 跳转到函数

#### Return 操作
1. 将返回值放到 ARG[0]
2. 恢复 SP = ARG + 1
3. 恢复 THAT, THIS, ARG, LCL
4. 跳转到返回地址

### 标签作用域

标签使用函数名前缀确保唯一性：
- `label LOOP` → `(functionName$LOOP)`
- `goto LOOP` → `@functionName$LOOP`

### 内存映射

```
RAM[0]      - SP (栈指针)
RAM[1]      - LCL (局部变量段基址)
RAM[2]      - ARG (参数段基址)
RAM[3]      - THIS (this 段基址)
RAM[4]      - THAT (that 段基址)
RAM[5-12]   - temp 段
RAM[13-15]  - 通用寄存器（用于实现）
RAM[16-255] - 静态变量
RAM[256+]   - 栈
```

## 测试文件说明

### 基础测试（单文件，无启动代码）

1. **BasicLoop** - 测试 label, goto, if-goto
2. **FibonacciSeries** - 测试循环和数组操作
3. **SimpleFunction** - 测试 function 和 return

### 完整测试（目录，有启动代码）

4. **FibonacciElement** - 递归函数调用
5. **StaticsTest** - 多文件 static 变量
6. **NestedCall** - 嵌套函数调用

## 清理

删除编译生成的文件：

```bash
make clean
```

## Coursera 提交要求

根据 Coursera 要求：
- ✅ 翻译单个文件时：**不生成启动代码**（用于基础测试）
- ✅ 翻译目录时：**生成启动代码**（用于完整程序测试）

这个实现完全符合在线评分器的要求。

## 示例

### 单文件翻译（无启动代码）

输入（BasicLoop.vm）：
```vm
push constant 0
pop local 0
label LOOP
push argument 0
push local 0
add
pop local 0
push argument 0
push constant 1
sub
pop argument 0
push argument 0
if-goto LOOP
push local 0
```

输出：生成 BasicLoop.asm（不包含启动代码）

### 目录翻译（有启动代码）

输入：FibonacciElement 目录包含：
- Main.vm
- Sys.vm

输出：FibonacciElement.asm（包含启动代码 + 所有文件翻译）

## 注意事项

- 本翻译器实现了 Hack VM 的完整规范
- 支持递归函数调用
- 正确处理多文件项目中的 static 变量
- 所有标签都有函数作用域，避免冲突
