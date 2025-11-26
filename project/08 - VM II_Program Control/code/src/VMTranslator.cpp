#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include "Parser.h"
#include "CodeWriter.h"

// 检查路径是否是目录
bool isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

// 获取目录中所有的 .vm 文件
std::vector<std::string> getVMFiles(const std::string& dirPath) {
    std::vector<std::string> vmFiles;
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        return vmFiles;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename.length() > 3 && filename.substr(filename.length() - 3) == ".vm") {
            vmFiles.push_back(dirPath + "/" + filename);
        }
    }
    closedir(dir);
    return vmFiles;
}

// 翻译单个 VM 文件
void translateFile(const std::string& vmFile, CodeWriter& writer) {
    Parser parser(vmFile);
    writer.setFileName(vmFile);

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
        else if (cmdType == C_LABEL) {
            writer.writeLabel(parser.arg1());
        }
        else if (cmdType == C_GOTO) {
            writer.writeGoto(parser.arg1());
        }
        else if (cmdType == C_IF) {
            writer.writeIf(parser.arg1());
        }
        else if (cmdType == C_FUNCTION) {
            writer.writeFunction(parser.arg1(), parser.arg2());
        }
        else if (cmdType == C_CALL) {
            writer.writeCall(parser.arg1(), parser.arg2());
        }
        else if (cmdType == C_RETURN) {
            writer.writeReturn();
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <input.vm 或 directory>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    
    if (isDirectory(input)) {
        // 处理目录
        std::vector<std::string> vmFiles = getVMFiles(input);
        if (vmFiles.empty()) {
            std::cerr << "错误: 目录中没有找到 .vm 文件" << std::endl;
            return 1;
        }

        // 移除路径末尾的斜杠
        std::string dirPath = input;
        if (dirPath[dirPath.length() - 1] == '/') {
            dirPath = dirPath.substr(0, dirPath.length() - 1);
        }

        // 获取目录名作为输出文件名
        size_t lastSlash = dirPath.find_last_of("/\\");
        std::string dirName = (lastSlash == std::string::npos) 
                             ? dirPath 
                             : dirPath.substr(lastSlash + 1);
        
        std::string outputFile = dirPath + "/" + dirName + ".asm";
        CodeWriter writer(outputFile);

        // 写入启动代码（当翻译目录时）
        writer.writeInit();

        // 翻译所有 VM 文件
        for (const auto& vmFile : vmFiles) {
            std::cout << "翻译文件: " << vmFile << std::endl;
            translateFile(vmFile, writer);
        }

        writer.close();
        std::cout << "目录翻译成功！输出文件: " << outputFile << std::endl;
    }
    else {
        // 处理单个文件
        std::string outputFile = input.substr(0, input.find_last_of('.')) + ".asm";
        CodeWriter writer(outputFile);

        // 翻译单个文件（不生成启动代码）
        translateFile(input, writer);

        writer.close();
        std::cout << "翻译成功！输出文件: " << outputFile << std::endl;
    }

    return 0;
}
