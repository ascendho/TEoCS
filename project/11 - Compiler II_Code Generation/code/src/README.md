# Project 11: Compiler II - Code Generation

完整的Jack编译器实现，将Jack高级语言编译为VM代码。这是Project 10语法分析器的扩展版本。

## 项目结构

```
11 - Compiler II_Code Generation/
├── code/
│   ├── src/
│   │   ├── JackCompiler.cpp         # 主程序驱动
│   │   ├── JackTokenizer.h          # 词法分析器（复用自Project 10）
│   │   ├── CompilationEngineVM.h    # 代码生成引擎
│   │   ├── SymbolTable.h            # 符号表管理
│   │   ├── VMWriter.h               # VM代码输出
│   │   └── Makefile                 # 构建脚本
│   └── test/
│       ├── Seven/                   # 简单表达式测试
│       ├── ConvertToBin/            # 过程化语言测试
│       ├── Square/                  # 面向对象测试
│       ├── Average/                 # 数组和字符串测试
│       ├── Pong/                    # 完整游戏测试
│       └── ComplexArrays/           # 复杂数组测试
```

## 编译和使用

### 编译
```bash
cd src
make -f Makefile
```

### 使用方法
```bash
# 编译单个文件
./JackCompiler ../test/Seven/Main.jack

# 编译整个目录
./JackCompiler ../test/Seven

# 运行所有测试
make -f Makefile test
```

## 输出示例

**输入 (Seven/Main.jack)**:
```jack
class Main {
   function void main() {
      do Output.printInt(1 + (2 * 3));
      return;
   }
}
```

**输出 (Seven/Main.vm)**:
```
function Main.main 0
push constant 1
push constant 2
push constant 3
call Math.multiply 2
add
call Output.printInt 1
pop temp 0
push constant 0
return
```

## 实现特性

- ✅ 完整的Jack语法支持
- ✅ 符号表管理（类级别和子程序级别）
- ✅ VM代码生成
- ✅ 构造函数、方法、函数
- ✅ 数组和字符串处理
- ✅ 控制流语句
- ✅ 表达式求值

## 测试通过

所有6个测试程序编译成功：
- Seven ✓
- ConvertToBin ✓
- Square ✓
- Average ✓
- Pong ✓
- ComplexArrays ✓
