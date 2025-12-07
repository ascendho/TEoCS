#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>
#include <stdexcept>

enum class Segment {
    CONST,
    ARG,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

enum class Command {
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

class VMWriter {
private:
    std::ofstream output;
    
    std::string segmentToString(Segment segment) const {
        switch (segment) {
            case Segment::CONST: return "constant";
            case Segment::ARG: return "argument";
            case Segment::LOCAL: return "local";
            case Segment::STATIC: return "static";
            case Segment::THIS: return "this";
            case Segment::THAT: return "that";
            case Segment::POINTER: return "pointer";
            case Segment::TEMP: return "temp";
            default: return "";
        }
    }
    
    std::string commandToString(Command command) const {
        switch (command) {
            case Command::ADD: return "add";
            case Command::SUB: return "sub";
            case Command::NEG: return "neg";
            case Command::EQ: return "eq";
            case Command::GT: return "gt";
            case Command::LT: return "lt";
            case Command::AND: return "and";
            case Command::OR: return "or";
            case Command::NOT: return "not";
            default: return "";
        }
    }

public:
    VMWriter(const std::string& filename) {
        output.open(filename);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filename);
        }
    }
    
    ~VMWriter() {
        if (output.is_open()) {
            output.close();
        }
    }
    
    // Writes a VM push command
    void writePush(Segment segment, int index) {
        output << "push " << segmentToString(segment) << " " << index << "\n";
    }
    
    // Writes a VM pop command
    void writePop(Segment segment, int index) {
        output << "pop " << segmentToString(segment) << " " << index << "\n";
    }
    
    // Writes a VM arithmetic command
    void writeArithmetic(Command command) {
        output << commandToString(command) << "\n";
    }
    
    // Writes a VM label command
    void writeLabel(const std::string& label) {
        output << "label " << label << "\n";
    }
    
    // Writes a VM goto command
    void writeGoto(const std::string& label) {
        output << "goto " << label << "\n";
    }
    
    // Writes a VM if-goto command
    void writeIf(const std::string& label) {
        output << "if-goto " << label << "\n";
    }
    
    // Writes a VM call command
    void writeCall(const std::string& name, int nArgs) {
        output << "call " << name << " " << nArgs << "\n";
    }
    
    // Writes a VM function command
    void writeFunction(const std::string& name, int nLocals) {
        output << "function " << name << " " << nLocals << "\n";
    }
    
    // Writes a VM return command
    void writeReturn() {
        output << "return\n";
    }
    
    // Close the output file
    void close() {
        if (output.is_open()) {
            output.close();
        }
    }
};

#endif // VM_WRITER_H
