#include "JackTokenizer.h"
#include "CompilationEngineVM.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void compileFile(const std::string& inputFile) {
    try {
        std::string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".vm";
        
        JackTokenizer tokenizer(inputFile);
        
        if (tokenizer.hasMoreTokens()) {
            tokenizer.advance();
        }
        
        CompilationEngine engine(outputFile, tokenizer);
        engine.compileClass();
        
        std::cout << "Compiled: " << inputFile << " -> " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing " << inputFile << ": " << e.what() << std::endl;
    }
}

void compileDirectory(const std::string& dirPath) {
    std::vector<std::string> jackFiles;
    
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".jack") {
            jackFiles.push_back(entry.path().string());
        }
    }
    
    if (jackFiles.empty()) {
        std::cerr << "No .jack files found in directory: " << dirPath << std::endl;
        return;
    }
    
    for (const auto& file : jackFiles) {
        compileFile(file);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file.jack | directory>" << std::endl;
        return 1;
    }
    
    std::string path = argv[1];
    
    try {
        if (fs::is_directory(path)) {
            compileDirectory(path);
        } else if (fs::is_regular_file(path)) {
            if (path.substr(path.find_last_of('.')) != ".jack") {
                std::cerr << "Error: File must have .jack extension" << std::endl;
                return 1;
            }
            compileFile(path);
        } else {
            std::cerr << "Error: Invalid path: " << path << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
