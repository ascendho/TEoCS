# Project 10: Compiler I - Parsing

Jack语法分析器的C++实现，将Jack源代码解析为XML格式的语法树。

## 项目结构

```
10 - Compiler I_Parsing/
├── code/
│   ├── src/
│   │   ├── JackTokenizer.h      # 词法分析器
│   │   ├── CompilationEngine.h  # 语法分析引擎
│   │   ├── JackAnalyzer.cpp     # 主程序驱动
│   │   └── Makefile             # 构建脚本
│   └── test/
│       ├── ArrayTest/           # 测试案例1
│       ├── ExpressionLessSquare/ # 测试案例2
│       └── Square/              # 测试案例3
```

## 模块说明

### JackTokenizer.h
词法分析模块，负责：
- 读取Jack源文件
- 识别和分类tokens（关键字、符号、标识符、整数常量、字符串常量）
- 处理注释（单行 `//` 和多行 `/* */`、API注释 `/** */`）
- 跳过空白字符

### CompilationEngine.h
语法分析模块，负责：
- 解析Jack语言的语法结构
- 生成XML格式的解析树
- 实现递归下降解析算法
- 支持的语法元素：
  - 类声明（class）
  - 变量声明（classVarDec, varDec）
  - 子程序声明（subroutineDec）
  - 语句（let, if, while, do, return）
  - 表达式（expression, term）

### JackAnalyzer.cpp
主驱动程序，负责：
- 接受命令行参数（单个.jack文件或包含.jack文件的目录）
- 为每个Jack文件生成两个XML文件：
  - `xxxT.xml` - tokens文件（词法分析结果）
  - `xxx.xml` - 解析树文件（语法分析结果）

## 编译和使用

### 编译
```bash
cd src
make
```

### 使用方法
```bash
# 分析单个文件
./JackAnalyzer ../test/ArrayTest/Main.jack

# 分析整个目录
./JackAnalyzer ../test/ArrayTest
```

### 测试所有案例
```bash
make test
```

### 清理生成文件
```bash
make clean
```

## 输出格式

对于每个`.jack`文件，生成器会创建：

1. **xxxT.xml** - Token列表
```xml
<tokens>
<keyword> class </keyword>
<identifier> Main </identifier>
<symbol> { </symbol>
...
</tokens>
```

2. **xxx.xml** - 语法树
```xml
<class>
  <keyword> class </keyword>
  <identifier> Main </identifier>
  <symbol> { </symbol>
  <subroutineDec>
    ...
  </subroutineDec>
</class>
```

## Jack语言特性支持

- ✅ 类声明和类变量（static, field）
- ✅ 子程序声明（constructor, function, method）
- ✅ 参数列表
- ✅ 局部变量声明（var）
- ✅ 语句（let, if, else, while, do, return）
- ✅ 表达式和运算符
- ✅ 数组访问 `array[index]`
- ✅ 方法调用 `object.method()` 和 `function()`
- ✅ 关键字常量（true, false, null, this）

## 技术细节

- **语言标准**: C++17
- **编译器**: g++ with `-std=c++17 -Wall -Wextra -O2`
- **依赖**: C++17 filesystem库
- **错误处理**: 异常捕获和详细错误信息

## 注意事项

1. XML中的特殊字符会被转义：
   - `<` → `&lt;`
   - `>` → `&gt;`
   - `&` → `&amp;`
   - `"` → `&quot;`

2. 字符串常量的引号不包含在输出中

3. 生成的XML使用2空格缩进

## 课程要求

本实现符合《计算机系统要素》（Nand to Tetris）课程Project 10的要求，完成了Jack编译器的第一阶段：语法分析。
