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

test: $(TARGET)
	@echo "=== 测试 SimpleAdd ==="
	./$(TARGET) ../test/SimpleAdd/SimpleAdd.vm
	@echo ""
	@echo "=== 测试 StackTest ==="
	./$(TARGET) ../test/StackTest/StackTest.vm
	@echo ""
	@echo "=== 测试 BasicTest ==="
	./$(TARGET) ../test/BasicTest/BasicTest.vm
	@echo ""
	@echo "=== 测试 PointerTest ==="
	./$(TARGET) ../test/PointerTest/PointerTest.vm
	@echo ""
	@echo "=== 测试 StaticTest ==="
	./$(TARGET) ../test/StaticTest/StaticTest.vm
	@echo ""
	@echo "所有测试文件已翻译完成！"

.PHONY: all clean test
