#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void analyzeFile(const std::string& inputFile) {
    try {
        std::string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".xml";
        std::string tokensFile = inputFile.substr(0, inputFile.find_last_of('.')) + "T.xml";
        
        // First pass: Generate tokens file
        {
            JackTokenizer tokenizer(inputFile);
            std::ofstream tokensOutput(tokensFile);
            
            if (!tokensOutput.is_open()) {
                throw std::runtime_error("Cannot create tokens file: " + tokensFile);
            }
            
            tokensOutput << "<tokens>\n";
            
            while (tokenizer.hasMoreTokens()) {
                tokenizer.advance();
                
                TokenType type = tokenizer.tokenType();
                if (type == TokenType::KEYWORD) {
                    tokensOutput << "<keyword> " << tokenizer.getCurrentToken() << " </keyword>\n";
                } else if (type == TokenType::SYMBOL) {
                    char sym = tokenizer.symbol();
                    std::string symStr;
                    if (sym == '<') symStr = "&lt;";
                    else if (sym == '>') symStr = "&gt;";
                    else if (sym == '&') symStr = "&amp;";
                    else if (sym == '"') symStr = "&quot;";
                    else symStr = sym;
                    tokensOutput << "<symbol> " << symStr << " </symbol>\n";
                } else if (type == TokenType::IDENTIFIER) {
                    tokensOutput << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
                } else if (type == TokenType::INT_CONST) {
                    tokensOutput << "<integerConstant> " << tokenizer.intVal() 
                               << " </integerConstant>\n";
                } else if (type == TokenType::STRING_CONST) {
                    tokensOutput << "<stringConstant> " << tokenizer.stringVal() 
                               << " </stringConstant>\n";
                }
            }
            
            tokensOutput << "</tokens>\n";
            tokensOutput.close();
        }
        
        // Second pass: Parse and generate XML
        {
            JackTokenizer tokenizer(inputFile);
            
            if (tokenizer.hasMoreTokens()) {
                tokenizer.advance();
            }
            
            CompilationEngine engine(inputFile, outputFile, tokenizer);
            engine.compileClass();
        }
        
        std::cout << "Compiled: " << inputFile << " -> " << outputFile << std::endl;
        std::cout << "Tokens: " << tokensFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing " << inputFile << ": " << e.what() << std::endl;
    }
}

void analyzeDirectory(const std::string& dirPath) {
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
        analyzeFile(file);
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
            analyzeDirectory(path);
        } else if (fs::is_regular_file(path)) {
            if (path.substr(path.find_last_of('.')) != ".jack") {
                std::cerr << "Error: File must have .jack extension" << std::endl;
                return 1;
            }
            analyzeFile(path);
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
