#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>

class CodeWriter {
private:
    std::ofstream outFile;
    std::string currentFileName;
    std::string currentFunctionName;  // 当前函数名
    int labelCounter;  // 用于生成唯一标签
    int callCounter;   // 用于生成唯一的返回地址标签

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
        callCounter = 0;
        currentFunctionName = "";
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

    // 写入初始化代码（启动代码）
    void writeInit() {
        writeComment("Bootstrap code");
        // 初始化 SP = 256
        outFile << "@256" << std::endl;
        outFile << "D=A" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=D" << std::endl;
        // 调用 Sys.init
        writeCall("Sys.init", 0);
    }

    // 写入 label 命令
    void writeLabel(const std::string& label) {
        writeComment("label " + label);
        outFile << "(" << currentFunctionName << "$" << label << ")" << std::endl;
    }

    // 写入 goto 命令
    void writeGoto(const std::string& label) {
        writeComment("goto " + label);
        outFile << "@" << currentFunctionName << "$" << label << std::endl;
        outFile << "0;JMP" << std::endl;
    }

    // 写入 if-goto 命令
    void writeIf(const std::string& label) {
        writeComment("if-goto " + label);
        outFile << "@SP" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@" << currentFunctionName << "$" << label << std::endl;
        outFile << "D;JNE" << std::endl;
    }

    // 写入 function 命令
    void writeFunction(const std::string& functionName, int nVars) {
        writeComment("function " + functionName + " " + std::to_string(nVars));
        currentFunctionName = functionName;
        
        // 声明函数入口标签
        outFile << "(" << functionName << ")" << std::endl;
        
        // 初始化局部变量为 0
        for (int i = 0; i < nVars; i++) {
            outFile << "@SP" << std::endl;
            outFile << "A=M" << std::endl;
            outFile << "M=0" << std::endl;
            outFile << "@SP" << std::endl;
            outFile << "M=M+1" << std::endl;
        }
    }

    // 写入 call 命令
    void writeCall(const std::string& functionName, int nArgs) {
        writeComment("call " + functionName + " " + std::to_string(nArgs));
        std::string returnLabel = "RETURN_" + std::to_string(callCounter++);
        
        // push return-address
        outFile << "@" << returnLabel << std::endl;
        outFile << "D=A" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=M+1" << std::endl;
        
        // push LCL
        outFile << "@LCL" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=M+1" << std::endl;
        
        // push ARG
        outFile << "@ARG" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=M+1" << std::endl;
        
        // push THIS
        outFile << "@THIS" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=M+1" << std::endl;
        
        // push THAT
        outFile << "@THAT" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=M+1" << std::endl;
        
        // ARG = SP - 5 - nArgs
        outFile << "@SP" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@" << (5 + nArgs) << std::endl;
        outFile << "D=D-A" << std::endl;
        outFile << "@ARG" << std::endl;
        outFile << "M=D" << std::endl;
        
        // LCL = SP
        outFile << "@SP" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@LCL" << std::endl;
        outFile << "M=D" << std::endl;
        
        // goto functionName
        outFile << "@" << functionName << std::endl;
        outFile << "0;JMP" << std::endl;
        
        // (return-address)
        outFile << "(" << returnLabel << ")" << std::endl;
    }

    // 写入 return 命令
    void writeReturn() {
        writeComment("return");
        
        // frame = LCL (R13 = frame)
        outFile << "@LCL" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@R13" << std::endl;
        outFile << "M=D" << std::endl;
        
        // retAddr = *(frame - 5) (R14 = return address)
        outFile << "@5" << std::endl;
        outFile << "A=D-A" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@R14" << std::endl;
        outFile << "M=D" << std::endl;
        
        // *ARG = pop()
        outFile << "@SP" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@ARG" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "M=D" << std::endl;
        
        // SP = ARG + 1
        outFile << "@ARG" << std::endl;
        outFile << "D=M+1" << std::endl;
        outFile << "@SP" << std::endl;
        outFile << "M=D" << std::endl;
        
        // THAT = *(frame - 1)
        outFile << "@R13" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@THAT" << std::endl;
        outFile << "M=D" << std::endl;
        
        // THIS = *(frame - 2)
        outFile << "@R13" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@THIS" << std::endl;
        outFile << "M=D" << std::endl;
        
        // ARG = *(frame - 3)
        outFile << "@R13" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@ARG" << std::endl;
        outFile << "M=D" << std::endl;
        
        // LCL = *(frame - 4)
        outFile << "@R13" << std::endl;
        outFile << "AM=M-1" << std::endl;
        outFile << "D=M" << std::endl;
        outFile << "@LCL" << std::endl;
        outFile << "M=D" << std::endl;
        
        // goto retAddr
        outFile << "@R14" << std::endl;
        outFile << "A=M" << std::endl;
        outFile << "0;JMP" << std::endl;
    }

    void close() {
        if (outFile.is_open()) {
            outFile.close();
        }
    }
};

#endif
