#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include <fstream>
#include <string>
#include <stdexcept>

class CompilationEngine {
private:
    JackTokenizer& tokenizer;
    std::ofstream output;
    int indentLevel;
    
    void writeIndent() {
        for (int i = 0; i < indentLevel; i++) {
            output << "  ";
        }
    }
    
    void writeOpenTag(const std::string& tag) {
        writeIndent();
        output << "<" << tag << ">\n";
        indentLevel++;
    }
    
    void writeCloseTag(const std::string& tag) {
        indentLevel--;
        writeIndent();
        output << "</" << tag << ">\n";
    }
    
    void writeTerminal(const std::string& tag, const std::string& value) {
        writeIndent();
        output << "<" << tag << "> " << xmlEscape(value) << " </" << tag << ">\n";
    }
    
    std::string xmlEscape(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                default: result += c;
            }
        }
        return result;
    }
    
    void eat(const std::string& expected) {
        if (tokenizer.getCurrentToken() != expected) {
            throw std::runtime_error("Expected '" + expected + "' but got '" + 
                                   tokenizer.getCurrentToken() + "'");
        }
        
        // Write the terminal token
        TokenType type = tokenizer.tokenType();
        if (type == TokenType::KEYWORD) {
            writeTerminal("keyword", tokenizer.getCurrentToken());
        } else if (type == TokenType::SYMBOL) {
            writeTerminal("symbol", std::string(1, tokenizer.symbol()));
        } else if (type == TokenType::IDENTIFIER) {
            writeTerminal("identifier", tokenizer.identifier());
        } else if (type == TokenType::INT_CONST) {
            writeTerminal("integerConstant", std::to_string(tokenizer.intVal()));
        } else if (type == TokenType::STRING_CONST) {
            writeTerminal("stringConstant", tokenizer.stringVal());
        }
        
        if (tokenizer.hasMoreTokens()) {
            tokenizer.advance();
        }
    }
    
    bool isType() {
        std::string token = tokenizer.getCurrentToken();
        return token == "int" || token == "char" || token == "boolean" || 
               tokenizer.tokenType() == TokenType::IDENTIFIER;
    }
    
    bool isStatement() {
        std::string token = tokenizer.getCurrentToken();
        return token == "let" || token == "if" || token == "while" || 
               token == "do" || token == "return";
    }
    
    bool isOp() {
        if (tokenizer.tokenType() != TokenType::SYMBOL) return false;
        char c = tokenizer.symbol();
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || 
               c == '|' || c == '<' || c == '>' || c == '=';
    }
    
    bool isUnaryOp() {
        if (tokenizer.tokenType() != TokenType::SYMBOL) return false;
        char c = tokenizer.symbol();
        return c == '-' || c == '~';
    }
    
    bool isKeywordConstant() {
        std::string token = tokenizer.getCurrentToken();
        return token == "true" || token == "false" || token == "null" || token == "this";
    }

public:
    CompilationEngine(const std::string& /* inputFile */, const std::string& outputFile, 
                     JackTokenizer& tok)
        : tokenizer(tok), indentLevel(0) {
        output.open(outputFile);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot open output file: " + outputFile);
        }
    }
    
    ~CompilationEngine() {
        if (output.is_open()) {
            output.close();
        }
    }
    
    void compileClass() {
        writeOpenTag("class");
        
        eat("class");
        eat(tokenizer.getCurrentToken()); // className
        eat("{");
        
        // classVarDec*
        while (tokenizer.getCurrentToken() == "static" || 
               tokenizer.getCurrentToken() == "field") {
            compileClassVarDec();
        }
        
        // subroutineDec*
        while (tokenizer.getCurrentToken() == "constructor" || 
               tokenizer.getCurrentToken() == "function" || 
               tokenizer.getCurrentToken() == "method") {
            compileSubroutine();
        }
        
        eat("}");
        
        writeCloseTag("class");
    }
    
    void compileClassVarDec() {
        writeOpenTag("classVarDec");
        
        eat(tokenizer.getCurrentToken()); // 'static' | 'field'
        eat(tokenizer.getCurrentToken()); // type
        eat(tokenizer.getCurrentToken()); // varName
        
        while (tokenizer.getCurrentToken() == ",") {
            eat(",");
            eat(tokenizer.getCurrentToken()); // varName
        }
        
        eat(";");
        
        writeCloseTag("classVarDec");
    }
    
    void compileSubroutine() {
        writeOpenTag("subroutineDec");
        
        eat(tokenizer.getCurrentToken()); // 'constructor' | 'function' | 'method'
        eat(tokenizer.getCurrentToken()); // 'void' | type
        eat(tokenizer.getCurrentToken()); // subroutineName
        eat("(");
        compileParameterList();
        eat(")");
        
        // subroutineBody
        writeOpenTag("subroutineBody");
        eat("{");
        
        // varDec*
        while (tokenizer.getCurrentToken() == "var") {
            compileVarDec();
        }
        
        compileStatements();
        eat("}");
        writeCloseTag("subroutineBody");
        
        writeCloseTag("subroutineDec");
    }
    
    void compileParameterList() {
        writeOpenTag("parameterList");
        
        if (isType()) {
            eat(tokenizer.getCurrentToken()); // type
            eat(tokenizer.getCurrentToken()); // varName
            
            while (tokenizer.getCurrentToken() == ",") {
                eat(",");
                eat(tokenizer.getCurrentToken()); // type
                eat(tokenizer.getCurrentToken()); // varName
            }
        }
        
        writeCloseTag("parameterList");
    }
    
    void compileVarDec() {
        writeOpenTag("varDec");
        
        eat("var");
        eat(tokenizer.getCurrentToken()); // type
        eat(tokenizer.getCurrentToken()); // varName
        
        while (tokenizer.getCurrentToken() == ",") {
            eat(",");
            eat(tokenizer.getCurrentToken()); // varName
        }
        
        eat(";");
        
        writeCloseTag("varDec");
    }
    
    void compileStatements() {
        writeOpenTag("statements");
        
        while (isStatement()) {
            std::string token = tokenizer.getCurrentToken();
            if (token == "let") {
                compileLet();
            } else if (token == "if") {
                compileIf();
            } else if (token == "while") {
                compileWhile();
            } else if (token == "do") {
                compileDo();
            } else if (token == "return") {
                compileReturn();
            }
        }
        
        writeCloseTag("statements");
    }
    
    void compileDo() {
        writeOpenTag("doStatement");
        
        eat("do");
        eat(tokenizer.getCurrentToken()); // subroutineName | className | varName
        
        if (tokenizer.getCurrentToken() == ".") {
            eat(".");
            eat(tokenizer.getCurrentToken()); // subroutineName
        }
        
        eat("(");
        compileExpressionList();
        eat(")");
        eat(";");
        
        writeCloseTag("doStatement");
    }
    
    void compileLet() {
        writeOpenTag("letStatement");
        
        eat("let");
        eat(tokenizer.getCurrentToken()); // varName
        
        if (tokenizer.getCurrentToken() == "[") {
            eat("[");
            compileExpression();
            eat("]");
        }
        
        eat("=");
        compileExpression();
        eat(";");
        
        writeCloseTag("letStatement");
    }
    
    void compileWhile() {
        writeOpenTag("whileStatement");
        
        eat("while");
        eat("(");
        compileExpression();
        eat(")");
        eat("{");
        compileStatements();
        eat("}");
        
        writeCloseTag("whileStatement");
    }
    
    void compileReturn() {
        writeOpenTag("returnStatement");
        
        eat("return");
        
        if (tokenizer.getCurrentToken() != ";") {
            compileExpression();
        }
        
        eat(";");
        
        writeCloseTag("returnStatement");
    }
    
    void compileIf() {
        writeOpenTag("ifStatement");
        
        eat("if");
        eat("(");
        compileExpression();
        eat(")");
        eat("{");
        compileStatements();
        eat("}");
        
        if (tokenizer.getCurrentToken() == "else") {
            eat("else");
            eat("{");
            compileStatements();
            eat("}");
        }
        
        writeCloseTag("ifStatement");
    }
    
    void compileExpression() {
        writeOpenTag("expression");
        
        compileTerm();
        
        while (isOp()) {
            eat(std::string(1, tokenizer.symbol())); // op
            compileTerm();
        }
        
        writeCloseTag("expression");
    }
    
    void compileTerm() {
        writeOpenTag("term");
        
        if (tokenizer.tokenType() == TokenType::INT_CONST) {
            eat(tokenizer.getCurrentToken());
        } else if (tokenizer.tokenType() == TokenType::STRING_CONST) {
            eat(tokenizer.getCurrentToken());
        } else if (isKeywordConstant()) {
            eat(tokenizer.getCurrentToken());
        } else if (tokenizer.getCurrentToken() == "(") {
            eat("(");
            compileExpression();
            eat(")");
        } else if (isUnaryOp()) {
            eat(std::string(1, tokenizer.symbol()));
            compileTerm();
        } else {
            // Must be varName, varName[expression], or subroutineCall
            eat(tokenizer.getCurrentToken()); // varName | className | subroutineName
            
            if (tokenizer.getCurrentToken() == "[") {
                eat("[");
                compileExpression();
                eat("]");
            } else if (tokenizer.getCurrentToken() == "(") {
                eat("(");
                compileExpressionList();
                eat(")");
            } else if (tokenizer.getCurrentToken() == ".") {
                eat(".");
                eat(tokenizer.getCurrentToken()); // subroutineName
                eat("(");
                compileExpressionList();
                eat(")");
            }
        }
        
        writeCloseTag("term");
    }
    
    void compileExpressionList() {
        writeOpenTag("expressionList");
        
        if (tokenizer.getCurrentToken() != ")") {
            compileExpression();
            
            while (tokenizer.getCurrentToken() == ",") {
                eat(",");
                compileExpression();
            }
        }
        
        writeCloseTag("expressionList");
    }
};

#endif // COMPILATION_ENGINE_H
