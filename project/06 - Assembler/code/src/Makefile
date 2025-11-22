CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = assembler
SOURCES = assembler.cpp
HEADERS = Parser.h Code.h SymbolTable.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) ../test/Add.asm
	./$(TARGET) ../test/Max.asm
	./$(TARGET) ../test/Rect.asm
	./$(TARGET) ../test/Pong.asm
	mv ../test/*.hack ../binary/

.PHONY: all clean test
