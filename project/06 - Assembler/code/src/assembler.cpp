// cd "project/06 - Assembler/code/cpp"
// make                          # 编译
// ./assembler ../test/Add.asm   # 汇编单个文件
// make test                     # 汇编所有测试文件

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"

std::string decimalToBinary(int decimal) {
    return std::bitset<16>(decimal).to_string();
}

bool isNumber(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <input.asm>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".hack";

    // 初始化符号表
    SymbolTable symbolTable;
    Code code;

    // 第一次遍历：构建符号表（处理标签）
    Parser firstPass(inputFile);
    int romAddress = 0;

    while (firstPass.hasMoreCommands()) {
        firstPass.advance();
        
        // 跳过空命令
        if (firstPass.getCurrentCommand().empty()) {
            break;
        }
        
        CommandType type = firstPass.commandType();

        if (type == L_COMMAND) {
            // 标签声明，记录下一条指令的地址
            std::string symbol = firstPass.symbol();
            symbolTable.addEntry(symbol, romAddress);
        } else if (type == A_COMMAND || type == C_COMMAND) {
            // 只有实际指令才增加地址
            romAddress++;
        }
    }

    // 第二次遍历：生成机器码
    Parser secondPass(inputFile);
    std::ofstream outFile(outputFile);

    if (!outFile.is_open()) {
        std::cerr << "无法创建输出文件: " << outputFile << std::endl;
        return 1;
    }

    int nextVariableAddress = 16; // 变量从 RAM[16] 开始分配
    bool firstLine = true;

    while (secondPass.hasMoreCommands()) {
        secondPass.advance();
        
        // 跳过空命令
        if (secondPass.getCurrentCommand().empty()) {
            break;
        }
        
        CommandType type = secondPass.commandType();

        if (type == A_COMMAND) {
            std::string symbol = secondPass.symbol();
            int address;

            if (isNumber(symbol)) {
                // 直接是数字
                address = std::stoi(symbol);
            } else if (symbolTable.contains(symbol)) {
                // 已知符号（预定义或标签）
                address = symbolTable.getAddress(symbol);
            } else {
                // 新变量
                symbolTable.addEntry(symbol, nextVariableAddress);
                address = nextVariableAddress;
                nextVariableAddress++;
            }

            if (!firstLine) outFile << "\n";
            outFile << decimalToBinary(address);
            firstLine = false;

        } else if (type == C_COMMAND) {
            std::string destStr = secondPass.dest();
            std::string compStr = secondPass.comp();
            std::string jumpStr = secondPass.jump();

            std::string binaryCode = "111" + 
                                    code.comp(compStr) + 
                                    code.dest(destStr) + 
                                    code.jump(jumpStr);

            if (!firstLine) outFile << "\n";
            outFile << binaryCode;
            firstLine = false;
        }
        // L_COMMAND 不生成代码
    }

    outFile.close();
    std::cout << "汇编成功！输出文件: " << outputFile << std::endl;

    return 0;
}
