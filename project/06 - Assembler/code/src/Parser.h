#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <cctype>

enum CommandType {
    A_COMMAND,  // @Xxx
    C_COMMAND,  // dest=comp;jump
    L_COMMAND   // (Xxx)
};

class Parser {
private:
    std::ifstream file;
    std::string currentCommand;
    CommandType currentType;

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
            return C_COMMAND; // 默认
        }
        
        if (currentCommand[0] == '@') {
            return A_COMMAND;
        } else if (currentCommand[0] == '(') {
            return L_COMMAND;
        } else {
            return C_COMMAND;
        }
    }

    std::string symbol() {
        // 用于 A_COMMAND 和 L_COMMAND
        if (currentCommand[0] == '@') {
            return currentCommand.substr(1);
        } else if (currentCommand[0] == '(') {
            return currentCommand.substr(1, currentCommand.length() - 2);
        }
        return "";
    }

    std::string dest() {
        // 用于 C_COMMAND
        size_t pos = currentCommand.find('=');
        if (pos != std::string::npos) {
            return currentCommand.substr(0, pos);
        }
        return "";
    }

    std::string comp() {
        // 用于 C_COMMAND
        size_t equalPos = currentCommand.find('=');
        size_t semiPos = currentCommand.find(';');
        
        size_t start = (equalPos != std::string::npos) ? equalPos + 1 : 0;
        size_t end = (semiPos != std::string::npos) ? semiPos : currentCommand.length();
        
        return currentCommand.substr(start, end - start);
    }

    std::string jump() {
        // 用于 C_COMMAND
        size_t pos = currentCommand.find(';');
        if (pos != std::string::npos) {
            return currentCommand.substr(pos + 1);
        }
        return "";
    }

    void reset() {
        file.clear();
        file.seekg(0, std::ios::beg);
    }

    std::string getCurrentCommand() const {
        return currentCommand;
    }
};

#endif
