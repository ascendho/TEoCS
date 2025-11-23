# VM Translator (Nand to Tetris Project 7)

这是一个用 C++ 编写的 VM 翻译器，用于将 Hack VM 语言翻译成 Hack 汇编语言。

## 项目结构

```
src/
├── VMTranslator.cpp   # 主程序
├── Parser.h           # VM 命令解析器
├── CodeWriter.h       # 汇编代码生成器
├── Makefile           # 编译配置
└── VMTranslator       # 编译后的可执行文件
```

## 功能特性

### 支持的 VM 命令

#### 1. 算术/逻辑命令
- `add` - 加法
- `sub` - 减法
- `neg` - 取负
- `eq` - 等于
- `gt` - 大于
- `lt` - 小于
- `and` - 逻辑与
- `or` - 逻辑或
- `not` - 逻辑非

#### 2. 内存访问命令
- `push segment index` - 将值压入栈
- `pop segment index` - 从栈中弹出值

#### 3. 支持的内存段
- `constant` - 常量段
- `local` - 局部变量段
- `argument` - 参数段
- `this` - this 指针段
- `that` - that 指针段
- `temp` - 临时变量段 (RAM[5-12])
- `pointer` - 指针段 (THIS/THAT)
- `static` - 静态变量段

## 编译

在 `src` 目录下运行：

```bash
make
```

## 使用方法

### 翻译单个 VM 文件

```bash
./VMTranslator <input.vm>
```

例如：
```bash
./VMTranslator ../test/SimpleAdd/SimpleAdd.vm
```

这将在同一目录下生成 `SimpleAdd.asm` 文件。

### 运行所有测试

```bash
make test
```

这会自动翻译所有测试文件：
- SimpleAdd.vm
- StackTest.vm
- BasicTest.vm
- PointerTest.vm
- StaticTest.vm

## 实现细节

### 模块说明

#### Parser.h
负责解析 VM 命令：
- 读取 VM 文件并移除注释和空行
- 识别命令类型（算术、push、pop 等）
- 提取命令参数

#### CodeWriter.h
负责生成 Hack 汇编代码：
- 将算术/逻辑命令翻译为汇编指令
- 将内存访问命令翻译为汇编指令
- 生成唯一标签（用于比较操作）

#### VMTranslator.cpp
主程序：
- 协调 Parser 和 CodeWriter
- 处理输入/输出文件

### 栈操作

VM 使用栈来进行所有计算：
- `SP (RAM[0])` - 栈指针，指向栈顶的下一个空位
- 栈从 `RAM[256]` 开始

### 内存映射

```
RAM[0]      - SP (栈指针)
RAM[1]      - LCL (局部变量段基址)
RAM[2]      - ARG (参数段基址)
RAM[3]      - THIS (this 段基址)
RAM[4]      - THAT (that 段基址)
RAM[5-12]   - temp 段
RAM[13-15]  - 通用寄存器
RAM[16-255] - 静态变量
RAM[256+]   - 栈
```

## 测试

项目包含以下测试文件：

1. **SimpleAdd** - 简单加法测试
2. **StackTest** - 栈操作和算术/逻辑命令测试
3. **BasicTest** - 基本内存段测试
4. **PointerTest** - pointer/this/that 段测试
5. **StaticTest** - static 段测试

使用 Nand2Tetris 提供的 CPU Emulator 可以测试生成的 `.asm` 文件。

## 清理

删除编译生成的文件：

```bash
make clean
```

## 示例

输入（SimpleAdd.vm）：
```vm
push constant 7
push constant 8
add
```

输出（SimpleAdd.asm）：
```asm
// push constant 7
@7
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 8
@8
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
AM=M-1
D=M
A=A-1
M=D+M
```

## 注意事项

- 本翻译器只实现 Project 7 所需的功能
- Project 8 将添加程序流程控制和函数调用功能
- 生成的汇编代码包含注释，便于调试
