# Hack 汇编器 (Nand to Tetris Project 6)

这是一个用 C++ 编写的 Hack 汇编器，用于将 Hack 汇编语言（.asm）翻译成 Hack 机器语言（.hack）。

## 项目结构

```
cpp/
├── assembler.cpp      # 主程序
├── Parser.h           # 解析器类
├── Code.h             # 代码生成器类
├── SymbolTable.h      # 符号表类
├── Makefile           # 编译配置
└── assembler          # 编译后的可执行文件
```

## 编译

在 `cpp` 目录下运行：

```bash
make
```

## 使用方法

### 汇编单个文件

```bash
./assembler <input.asm>
```

例如：
```bash
./assembler ../test/Add.asm
```

这将在同一目录下生成 `Add.hack` 文件。

### 汇编所有测试文件

```bash
make test
```

这会自动编译所有测试文件并将 .hack 文件移动到 `../binary/` 目录。

## 汇编器功能

### 支持的指令类型

1. **A 指令**：`@value` 或 `@symbol`
   - 例如：`@2`, `@sum`, `@LOOP`

2. **C 指令**：`dest=comp;jump`
   - 例如：`D=A`, `M=D+1`, `D;JGT`

3. **标签**：`(LABEL)`
   - 例如：`(LOOP)`, `(END)`

### 预定义符号

汇编器内置以下预定义符号：

- **虚拟寄存器**：`SP`, `LCL`, `ARG`, `THIS`, `THAT`
- **寄存器**：`R0` 到 `R15` (映射到 RAM[0] 到 RAM[15])
- **I/O 指针**：`SCREEN` (16384), `KBD` (24576)

### 符号解析

- 标签符号：在第一次遍历时解析
- 变量符号：从 RAM[16] 开始自动分配

## 实现细节

汇编器采用两遍扫描（two-pass）算法：

1. **第一遍**：构建符号表
   - 扫描所有标签声明 `(LABEL)`
   - 记录每个标签对应的 ROM 地址

2. **第二遍**：生成机器码
   - 将 A 指令转换为 16 位二进制地址
   - 将 C 指令转换为 16 位二进制指令（格式：111accccccdddjjj）
   - 处理变量符号并分配内存地址

## 测试文件

项目包含以下测试文件：

- `Add.asm` - 简单加法程序
- `Max.asm` - 求最大值程序
- `Rect.asm` - 绘制矩形程序
- `Pong.asm` - Pong 游戏程序（大型程序测试）

## 清理

删除编译生成的文件：

```bash
make clean
```

## 输出示例

输入（Add.asm）：
```asm
@2
D=A
@3
D=D+A
@0
M=D
```

输出（Add.hack）：
```
0000000000000010
1110110000010000
0000000000000011
1110000010010000
0000000000000000
1110001100001000
```
