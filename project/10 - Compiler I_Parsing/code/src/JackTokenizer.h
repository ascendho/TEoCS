#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <string>
#include <fstream>
#include <cctype>
#include <unordered_set>

enum class TokenType {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum class Keyword {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR,
    STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, KNULL, THIS
};

class JackTokenizer {
private:
    std::ifstream input;
    std::string currentToken;
    TokenType currentType;
    std::string currentLine;
    size_t currentPos;
    bool inMultiLineComment;
    
    static const std::unordered_set<std::string> keywords;
    static const std::unordered_set<char> symbols;
    
    void skipWhitespaceAndComments() {
        while (true) {
            // Skip whitespace
            while (currentPos < currentLine.length() && std::isspace(currentLine[currentPos])) {
                currentPos++;
            }
            
            // If end of line, read next line
            if (currentPos >= currentLine.length()) {
                if (!std::getline(input, currentLine)) {
                    return; // End of file
                }
                currentPos = 0;
                continue;
            }
            
            // Handle multi-line comments
            if (inMultiLineComment) {
                size_t endPos = currentLine.find("*/", currentPos);
                if (endPos != std::string::npos) {
                    inMultiLineComment = false;
                    currentPos = endPos + 2;
                    continue;
                } else {
                    // Still in comment, read next line
                    if (!std::getline(input, currentLine)) {
                        return;
                    }
                    currentPos = 0;
                    continue;
                }
            }
            
            // Check for single-line comment
            if (currentPos + 1 < currentLine.length() && 
                currentLine[currentPos] == '/' && currentLine[currentPos + 1] == '/') {
                // Read next line
                if (!std::getline(input, currentLine)) {
                    return;
                }
                currentPos = 0;
                continue;
            }
            
            // Check for multi-line comment start
            if (currentPos + 1 < currentLine.length() && 
                currentLine[currentPos] == '/' && currentLine[currentPos + 1] == '*') {
                inMultiLineComment = true;
                currentPos += 2;
                continue;
            }
            
            // Check for API comment /** ... */
            if (currentPos + 2 < currentLine.length() && 
                currentLine[currentPos] == '/' && 
                currentLine[currentPos + 1] == '*' && 
                currentLine[currentPos + 2] == '*') {
                inMultiLineComment = true;
                currentPos += 3;
                continue;
            }
            
            break;
        }
    }

public:
    JackTokenizer(const std::string& filename) 
        : currentPos(0), inMultiLineComment(false) {
        input.open(filename);
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
    }
    
    ~JackTokenizer() {
        if (input.is_open()) {
            input.close();
        }
    }
    
    bool hasMoreTokens() {
        skipWhitespaceAndComments();
        return currentPos < currentLine.length() || input.good();
    }
    
    void advance() {
        skipWhitespaceAndComments();
        
        if (currentPos >= currentLine.length()) {
            return;
        }
        
        char c = currentLine[currentPos];
        
        // Check for string constant
        if (c == '"') {
            currentType = TokenType::STRING_CONST;
            currentPos++; // Skip opening quote
            size_t endPos = currentLine.find('"', currentPos);
            if (endPos == std::string::npos) {
                throw std::runtime_error("Unterminated string constant");
            }
            currentToken = currentLine.substr(currentPos, endPos - currentPos);
            currentPos = endPos + 1; // Skip closing quote
            return;
        }
        
        // Check for symbol
        if (symbols.count(c)) {
            currentType = TokenType::SYMBOL;
            currentToken = c;
            currentPos++;
            return;
        }
        
        // Check for integer constant
        if (std::isdigit(c)) {
            currentType = TokenType::INT_CONST;
            size_t start = currentPos;
            while (currentPos < currentLine.length() && std::isdigit(currentLine[currentPos])) {
                currentPos++;
            }
            currentToken = currentLine.substr(start, currentPos - start);
            return;
        }
        
        // Must be keyword or identifier
        if (std::isalpha(c) || c == '_') {
            size_t start = currentPos;
            while (currentPos < currentLine.length() && 
                   (std::isalnum(currentLine[currentPos]) || currentLine[currentPos] == '_')) {
                currentPos++;
            }
            currentToken = currentLine.substr(start, currentPos - start);
            
            if (keywords.count(currentToken)) {
                currentType = TokenType::KEYWORD;
            } else {
                currentType = TokenType::IDENTIFIER;
            }
            return;
        }
        
        throw std::runtime_error("Invalid character: " + std::string(1, c));
    }
    
    TokenType tokenType() const {
        return currentType;
    }
    
    Keyword keyword() const {
        if (currentType != TokenType::KEYWORD) {
            throw std::runtime_error("Current token is not a keyword");
        }
        
        if (currentToken == "class") return Keyword::CLASS;
        if (currentToken == "method") return Keyword::METHOD;
        if (currentToken == "function") return Keyword::FUNCTION;
        if (currentToken == "constructor") return Keyword::CONSTRUCTOR;
        if (currentToken == "int") return Keyword::INT;
        if (currentToken == "boolean") return Keyword::BOOLEAN;
        if (currentToken == "char") return Keyword::CHAR;
        if (currentToken == "void") return Keyword::VOID;
        if (currentToken == "var") return Keyword::VAR;
        if (currentToken == "static") return Keyword::STATIC;
        if (currentToken == "field") return Keyword::FIELD;
        if (currentToken == "let") return Keyword::LET;
        if (currentToken == "do") return Keyword::DO;
        if (currentToken == "if") return Keyword::IF;
        if (currentToken == "else") return Keyword::ELSE;
        if (currentToken == "while") return Keyword::WHILE;
        if (currentToken == "return") return Keyword::RETURN;
        if (currentToken == "true") return Keyword::TRUE;
        if (currentToken == "false") return Keyword::FALSE;
        if (currentToken == "null") return Keyword::KNULL;
        if (currentToken == "this") return Keyword::THIS;
        
        throw std::runtime_error("Unknown keyword: " + currentToken);
    }
    
    char symbol() const {
        if (currentType != TokenType::SYMBOL) {
            throw std::runtime_error("Current token is not a symbol");
        }
        return currentToken[0];
    }
    
    std::string identifier() const {
        if (currentType != TokenType::IDENTIFIER) {
            throw std::runtime_error("Current token is not an identifier");
        }
        return currentToken;
    }
    
    int intVal() const {
        if (currentType != TokenType::INT_CONST) {
            throw std::runtime_error("Current token is not an integer constant");
        }
        return std::stoi(currentToken);
    }
    
    std::string stringVal() const {
        if (currentType != TokenType::STRING_CONST) {
            throw std::runtime_error("Current token is not a string constant");
        }
        return currentToken;
    }
    
    std::string getCurrentToken() const {
        return currentToken;
    }
};

// Initialize static members
const std::unordered_set<std::string> JackTokenizer::keywords = {
    "class", "constructor", "function", "method", "field", "static",
    "var", "int", "char", "boolean", "void", "true", "false", "null",
    "this", "let", "do", "if", "else", "while", "return"
};

const std::unordered_set<char> JackTokenizer::symbols = {
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/',
    '&', '|', '<', '>', '=', '~'
};

#endif // JACK_TOKENIZER_H
