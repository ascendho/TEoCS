CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = JackAnalyzer
SRCDIR = .
TESTDIR = ../test

SOURCES = $(SRCDIR)/JackAnalyzer.cpp
HEADERS = $(SRCDIR)/JackTokenizer.h $(SRCDIR)/CompilationEngine.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
	find $(TESTDIR) -name "*.xml" -type f -delete

test: $(TARGET)
	@echo "Testing ArrayTest..."
	./$(TARGET) $(TESTDIR)/ArrayTest
	@echo "\nTesting ExpressionLessSquare..."
	./$(TARGET) $(TESTDIR)/ExpressionLessSquare
	@echo "\nTesting Square..."
	./$(TARGET) $(TESTDIR)/Square

.PHONY: all clean test
