#include <iostream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <input.vm>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".asm";

    Parser parser(inputFile);
    CodeWriter writer(outputFile);

    // 设置当前文件名（用于 static 段）
    writer.setFileName(inputFile);

    // 翻译 VM 命令
    while (parser.hasMoreCommands()) {
        parser.advance();

        // 跳过空命令
        if (parser.getCurrentCommand().empty()) {
            break;
        }

        CommandType cmdType = parser.commandType();

        if (cmdType == C_ARITHMETIC) {
            writer.writeArithmetic(parser.arg1());
        }
        else if (cmdType == C_PUSH || cmdType == C_POP) {
            std::string command = (cmdType == C_PUSH) ? "push" : "pop";
            writer.writePushPop(command, parser.arg1(), parser.arg2());
        }
        // Project 7 不需要处理其他命令类型
    }

    writer.close();
    std::cout << "翻译成功！输出文件: " << outputFile << std::endl;

    return 0;
}
