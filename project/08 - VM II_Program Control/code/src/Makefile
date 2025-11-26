CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = VMTranslator
SOURCES = VMTranslator.cpp
HEADERS = Parser.h CodeWriter.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f ../test/*/*.asm

# 测试程序流程控制（单文件，无启动代码）
test-flow: $(TARGET)
	@echo "=== 测试 BasicLoop (单文件) ==="
	./$(TARGET) ../test/BasicLoop/BasicLoop.vm
	@echo ""
	@echo "=== 测试 FibonacciSeries (单文件) ==="
	./$(TARGET) ../test/FibonacciSeries/FibonacciSeries.vm
	@echo ""

# 测试函数调用（单文件，无启动代码）
test-simple-function: $(TARGET)
	@echo "=== 测试 SimpleFunction (单文件) ==="
	./$(TARGET) ../test/SimpleFunction/SimpleFunction.vm
	@echo ""

# 测试函数调用（目录，有启动代码）
test-fibonacci: $(TARGET)
	@echo "=== 测试 FibonacciElement (目录) ==="
	./$(TARGET) ../test/FibonacciElement
	@echo ""

test-statics: $(TARGET)
	@echo "=== 测试 StaticsTest (目录) ==="
	./$(TARGET) ../test/StaticsTest
	@echo ""

test-nested: $(TARGET)
	@echo "=== 测试 NestedCall (目录) ==="
	./$(TARGET) ../test/NestedCall
	@echo ""

# 运行所有测试
test: test-flow test-simple-function test-fibonacci test-statics test-nested
	@echo ""
	@echo "所有测试文件已翻译完成！"

.PHONY: all clean test test-flow test-simple-function test-fibonacci test-statics test-nested
