#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <cctype>

enum CommandType {
    C_ARITHMETIC,   // add, sub, neg, eq, gt, lt, and, or, not
    C_PUSH,         // push segment index
    C_POP,          // pop segment index
    C_LABEL,        // label symbol
    C_GOTO,         // goto symbol
    C_IF,           // if-goto symbol
    C_FUNCTION,     // function functionName nArgs
    C_RETURN,       // return
    C_CALL          // call functionName nArgs
};

class Parser {
private:
    std::ifstream file;
    std::string currentCommand;

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    std::string removeComments(const std::string& line) {
        size_t pos = line.find("//");
        if (pos != std::string::npos) {
            return line.substr(0, pos);
        }
        return line;
    }

public:
    Parser(const std::string& filename) {
        file.open(filename);
    }

    ~Parser() {
        if (file.is_open()) {
            file.close();
        }
    }

    bool hasMoreCommands() {
        return !file.eof();
    }

    void advance() {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(removeComments(line));
            if (!line.empty()) {
                currentCommand = line;
                return;
            }
        }
        currentCommand = "";
    }

    CommandType commandType() {
        if (currentCommand.empty()) {
            return C_ARITHMETIC;
        }

        // 提取第一个单词
        size_t pos = currentCommand.find(' ');
        std::string cmd = (pos != std::string::npos) 
                         ? currentCommand.substr(0, pos) 
                         : currentCommand;

        if (cmd == "push") return C_PUSH;
        if (cmd == "pop") return C_POP;
        if (cmd == "label") return C_LABEL;
        if (cmd == "goto") return C_GOTO;
        if (cmd == "if-goto") return C_IF;
        if (cmd == "function") return C_FUNCTION;
        if (cmd == "return") return C_RETURN;
        if (cmd == "call") return C_CALL;
        
        // 否则是算术/逻辑命令
        return C_ARITHMETIC;
    }

    std::string arg1() {
        // 对于 C_ARITHMETIC，返回命令本身
        // 对于其他命令，返回第一个参数
        CommandType type = commandType();
        
        if (type == C_ARITHMETIC) {
            return currentCommand;
        }

        // 提取第一个参数
        size_t firstSpace = currentCommand.find(' ');
        if (firstSpace == std::string::npos) return "";
        
        size_t secondSpace = currentCommand.find(' ', firstSpace + 1);
        if (secondSpace == std::string::npos) {
            return currentCommand.substr(firstSpace + 1);
        }
        
        return currentCommand.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    }

    int arg2() {
        // 只对 C_PUSH, C_POP, C_FUNCTION, C_CALL 有效
        size_t firstSpace = currentCommand.find(' ');
        if (firstSpace == std::string::npos) return 0;
        
        size_t secondSpace = currentCommand.find(' ', firstSpace + 1);
        if (secondSpace == std::string::npos) return 0;
        
        std::string arg = currentCommand.substr(secondSpace + 1);
        return std::stoi(arg);
    }

    std::string getCurrentCommand() const {
        return currentCommand;
    }

    void reset() {
        file.clear();
        file.seekg(0, std::ios::beg);
    }
};

#endif
