#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>

class CodeWriter {
private:
    std::ofstream outFile;
    std::string currentFileName;
    int labelCounter;  // 用于生成唯一标签

    // 写入注释
    void writeComment(const std::string& comment) {
        outFile << "// " << comment << std::endl;
    }

    // 生成唯一标签
    std::string getUniqueLabel(const std::string& base) {
        return base + std::to_string(labelCounter++);
    }

public:
    CodeWriter(const std::string& outputFile) {
        outFile.open(outputFile);
        labelCounter = 0;
    }

    ~CodeWriter() {
        if (outFile.is_open()) {
            outFile.close();
        }
    }

    void setFileName(const std::string& fileName) {
        // 提取不带路径和扩展名的文件名
        size_t lastSlash = fileName.find_last_of("/\\");
        size_t lastDot = fileName.find_last_of('.');
        
        if (lastSlash == std::string::npos) lastSlash = 0;
        else lastSlash++;
        
        if (lastDot == std::string::npos || lastDot < lastSlash) {
            currentFileName = fileName.substr(lastSlash);
        } else {
            currentFileName = fileName.substr(lastSlash, lastDot - lastSlash);
        }
    }

    // 写入算术/逻辑命令
    void writeArithmetic(const std::string& command) {
        writeComment(command);

        if (command == "add") {
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "M=D+M" << std::endl;
        }
        else if (command == "sub") {
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "M=M-D" << std::endl;
        }
        else if (command == "neg") {
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=-M" << std::endl;
        }
        else if (command == "eq") {
            std::string trueLabel = getUniqueLabel("EQ_TRUE");
            std::string endLabel = getUniqueLabel("EQ_END");
            
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "D=M-D" << std::endl;
            outFile << "@" << trueLabel << std::endl;
            outFile << "D;JEQ" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=0" << std::endl;
            outFile << "@" << endLabel << std::endl;
            outFile << "0;JMP" << std::endl;
            outFile << "(" << trueLabel << ")" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=-1" << std::endl;
            outFile << "(" << endLabel << ")" << std::endl;
        }
        else if (command == "gt") {
            std::string trueLabel = getUniqueLabel("GT_TRUE");
            std::string endLabel = getUniqueLabel("GT_END");
            
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "D=M-D" << std::endl;
            outFile << "@" << trueLabel << std::endl;
            outFile << "D;JGT" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=0" << std::endl;
            outFile << "@" << endLabel << std::endl;
            outFile << "0;JMP" << std::endl;
            outFile << "(" << trueLabel << ")" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=-1" << std::endl;
            outFile << "(" << endLabel << ")" << std::endl;
        }
        else if (command == "lt") {
            std::string trueLabel = getUniqueLabel("LT_TRUE");
            std::string endLabel = getUniqueLabel("LT_END");
            
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "D=M-D" << std::endl;
            outFile << "@" << trueLabel << std::endl;
            outFile << "D;JLT" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=0" << std::endl;
            outFile << "@" << endLabel << std::endl;
            outFile << "0;JMP" << std::endl;
            outFile << "(" << trueLabel << ")" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=-1" << std::endl;
            outFile << "(" << endLabel << ")" << std::endl;
        }
        else if (command == "and") {
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "M=D&M" << std::endl;
        }
        else if (command == "or") {
            outFile << "@SP" << std::endl;
            outFile << "AM=M-1" << std::endl;
            outFile << "D=M" << std::endl;
            outFile << "A=A-1" << std::endl;
            outFile << "M=D|M" << std::endl;
        }
        else if (command == "not") {
            outFile << "@SP" << std::endl;
            outFile << "A=M-1" << std::endl;
            outFile << "M=!M" << std::endl;
        }
    }

    // 写入 push/pop 命令
    void writePushPop(const std::string& command, const std::string& segment, int index) {
        writeComment(command + " " + segment + " " + std::to_string(index));

        if (command == "push") {
            if (segment == "constant") {
                outFile << "@" << index << std::endl;
                outFile << "D=A" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "M=M+1" << std::endl;
            }
            else if (segment == "local" || segment == "argument" || 
                     segment == "this" || segment == "that") {
                std::string segmentPointer;
                if (segment == "local") segmentPointer = "LCL";
                else if (segment == "argument") segmentPointer = "ARG";
                else if (segment == "this") segmentPointer = "THIS";
                else segmentPointer = "THAT";

                outFile << "@" << segmentPointer << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@" << index << std::endl;
                outFile << "A=D+A" << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "M=M+1" << std::endl;
            }
            else if (segment == "temp") {
                outFile << "@" << (5 + index) << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "M=M+1" << std::endl;
            }
            else if (segment == "pointer") {
                std::string pointer = (index == 0) ? "THIS" : "THAT";
                outFile << "@" << pointer << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "M=M+1" << std::endl;
            }
            else if (segment == "static") {
                outFile << "@" << currentFileName << "." << index << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "M=M+1" << std::endl;
            }
        }
        else if (command == "pop") {
            if (segment == "local" || segment == "argument" || 
                segment == "this" || segment == "that") {
                std::string segmentPointer;
                if (segment == "local") segmentPointer = "LCL";
                else if (segment == "argument") segmentPointer = "ARG";
                else if (segment == "this") segmentPointer = "THIS";
                else segmentPointer = "THAT";

                outFile << "@" << segmentPointer << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@" << index << std::endl;
                outFile << "D=D+A" << std::endl;
                outFile << "@R13" << std::endl;
                outFile << "M=D" << std::endl;
                outFile << "@SP" << std::endl;
                outFile << "AM=M-1" << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@R13" << std::endl;
                outFile << "A=M" << std::endl;
                outFile << "M=D" << std::endl;
            }
            else if (segment == "temp") {
                outFile << "@SP" << std::endl;
                outFile << "AM=M-1" << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@" << (5 + index) << std::endl;
                outFile << "M=D" << std::endl;
            }
            else if (segment == "pointer") {
                std::string pointer = (index == 0) ? "THIS" : "THAT";
                outFile << "@SP" << std::endl;
                outFile << "AM=M-1" << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@" << pointer << std::endl;
                outFile << "M=D" << std::endl;
            }
            else if (segment == "static") {
                outFile << "@SP" << std::endl;
                outFile << "AM=M-1" << std::endl;
                outFile << "D=M" << std::endl;
                outFile << "@" << currentFileName << "." << index << std::endl;
                outFile << "M=D" << std::endl;
            }
        }
    }

    void close() {
        if (outFile.is_open()) {
            outFile.close();
        }
    }
};

#endif
