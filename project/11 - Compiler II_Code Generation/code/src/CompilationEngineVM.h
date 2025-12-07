#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <string>
#include <stdexcept>

class CompilationEngine {
private:
    JackTokenizer& tokenizer;
    VMWriter vmWriter;
    SymbolTable symbolTable;
    std::string className;
    std::string currentSubroutineName;
    std::string currentSubroutineType; // "method", "function", or "constructor"
    int labelCounter;
    
    int getNextLabel() {
        return labelCounter++;
    }
    
    void eat(const std::string& expected) {
        if (tokenizer.getCurrentToken() != expected) {
            throw std::runtime_error("Expected '" + expected + "' but got '" + 
                                   tokenizer.getCurrentToken() + "'");
        }
        if (tokenizer.hasMoreTokens()) {
            tokenizer.advance();
        }
    }
    
    void eatIdentifier() {
        if (tokenizer.tokenType() != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected identifier but got '" + 
                                   tokenizer.getCurrentToken() + "'");
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
    
    Segment kindToSegment(VarKind kind) {
        switch (kind) {
            case VarKind::STATIC: return Segment::STATIC;
            case VarKind::FIELD: return Segment::THIS;
            case VarKind::ARG: return Segment::ARG;
            case VarKind::VAR: return Segment::LOCAL;
            default: throw std::runtime_error("Invalid variable kind");
        }
    }
    
    void compileOp(char op) {
        switch (op) {
            case '+': vmWriter.writeArithmetic(Command::ADD); break;
            case '-': vmWriter.writeArithmetic(Command::SUB); break;
            case '=': vmWriter.writeArithmetic(Command::EQ); break;
            case '>': vmWriter.writeArithmetic(Command::GT); break;
            case '<': vmWriter.writeArithmetic(Command::LT); break;
            case '&': vmWriter.writeArithmetic(Command::AND); break;
            case '|': vmWriter.writeArithmetic(Command::OR); break;
            case '*': vmWriter.writeCall("Math.multiply", 2); break;
            case '/': vmWriter.writeCall("Math.divide", 2); break;
            default: throw std::runtime_error("Unknown operator");
        }
    }

public:
    CompilationEngine(const std::string& outputFile, JackTokenizer& tok)
        : tokenizer(tok), vmWriter(outputFile), labelCounter(0) {}
    
    void compileClass() {
        eat("class");
        className = tokenizer.getCurrentToken();
        eatIdentifier();
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
    }
    
    void compileClassVarDec() {
        std::string kind = tokenizer.getCurrentToken(); // 'static' or 'field'
        VarKind varKind = (kind == "static") ? VarKind::STATIC : VarKind::FIELD;
        eat(kind);
        
        std::string type = tokenizer.getCurrentToken();
        eat(type);
        
        std::string name = tokenizer.getCurrentToken();
        symbolTable.define(name, type, varKind);
        eatIdentifier();
        
        while (tokenizer.getCurrentToken() == ",") {
            eat(",");
            name = tokenizer.getCurrentToken();
            symbolTable.define(name, type, varKind);
            eatIdentifier();
        }
        
        eat(";");
    }
    
    void compileSubroutine() {
        symbolTable.startSubroutine();
        
        currentSubroutineType = tokenizer.getCurrentToken();
        eat(currentSubroutineType); // 'constructor' | 'function' | 'method'
        
        eat(tokenizer.getCurrentToken()); // return type
        
        currentSubroutineName = tokenizer.getCurrentToken();
        eatIdentifier();
        
        // For methods, add 'this' as argument 0
        if (currentSubroutineType == "method") {
            symbolTable.define("this", className, VarKind::ARG);
        }
        
        eat("(");
        compileParameterList();
        eat(")");
        
        // subroutineBody
        eat("{");
        
        // varDec*
        while (tokenizer.getCurrentToken() == "var") {
            compileVarDec();
        }
        
        // Write function declaration
        int nLocals = symbolTable.varCount(VarKind::VAR);
        vmWriter.writeFunction(className + "." + currentSubroutineName, nLocals);
        
        // Handle constructor/method prologue
        if (currentSubroutineType == "constructor") {
            // Allocate memory for the object
            int nFields = symbolTable.varCount(VarKind::FIELD);
            vmWriter.writePush(Segment::CONST, nFields);
            vmWriter.writeCall("Memory.alloc", 1);
            vmWriter.writePop(Segment::POINTER, 0); // Set THIS
        } else if (currentSubroutineType == "method") {
            // Set THIS to argument 0
            vmWriter.writePush(Segment::ARG, 0);
            vmWriter.writePop(Segment::POINTER, 0);
        }
        
        compileStatements();
        eat("}");
    }
    
    void compileParameterList() {
        if (isType()) {
            std::string type = tokenizer.getCurrentToken();
            eat(type);
            std::string name = tokenizer.getCurrentToken();
            symbolTable.define(name, type, VarKind::ARG);
            eatIdentifier();
            
            while (tokenizer.getCurrentToken() == ",") {
                eat(",");
                type = tokenizer.getCurrentToken();
                eat(type);
                name = tokenizer.getCurrentToken();
                symbolTable.define(name, type, VarKind::ARG);
                eatIdentifier();
            }
        }
    }
    
    void compileVarDec() {
        eat("var");
        std::string type = tokenizer.getCurrentToken();
        eat(type);
        
        std::string name = tokenizer.getCurrentToken();
        symbolTable.define(name, type, VarKind::VAR);
        eatIdentifier();
        
        while (tokenizer.getCurrentToken() == ",") {
            eat(",");
            name = tokenizer.getCurrentToken();
            symbolTable.define(name, type, VarKind::VAR);
            eatIdentifier();
        }
        
        eat(";");
    }
    
    void compileStatements() {
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
    }
    
    void compileDo() {
        eat("do");
        
        std::string name = tokenizer.getCurrentToken();
        eatIdentifier();
        
        int nArgs = 0;
        std::string functionName;
        
        if (tokenizer.getCurrentToken() == ".") {
            eat(".");
            std::string methodName = tokenizer.getCurrentToken();
            eatIdentifier();
            
            // Check if name is a variable (object method call)
            if (symbolTable.exists(name)) {
                VarKind kind = symbolTable.kindOf(name);
                int index = symbolTable.indexOf(name);
                vmWriter.writePush(kindToSegment(kind), index);
                functionName = symbolTable.typeOf(name) + "." + methodName;
                nArgs = 1; // 'this' is the first argument
            } else {
                // Function or constructor call
                functionName = name + "." + methodName;
            }
        } else {
            // Method call in the same class
            vmWriter.writePush(Segment::POINTER, 0); // Push 'this'
            functionName = className + "." + name;
            nArgs = 1;
        }
        
        eat("(");
        nArgs += compileExpressionList();
        eat(")");
        eat(";");
        
        vmWriter.writeCall(functionName, nArgs);
        vmWriter.writePop(Segment::TEMP, 0); // Discard return value
    }
    
    void compileLet() {
        eat("let");
        
        std::string varName = tokenizer.getCurrentToken();
        eatIdentifier();
        
        bool isArray = false;
        if (tokenizer.getCurrentToken() == "[") {
            isArray = true;
            eat("[");
            
            // Push array base address
            VarKind kind = symbolTable.kindOf(varName);
            int index = symbolTable.indexOf(varName);
            vmWriter.writePush(kindToSegment(kind), index);
            
            compileExpression(); // Index expression
            vmWriter.writeArithmetic(Command::ADD); // base + index
            
            eat("]");
        }
        
        eat("=");
        compileExpression();
        eat(";");
        
        if (isArray) {
            // Pop value to temp, set THAT to address, pop to THAT 0
            vmWriter.writePop(Segment::TEMP, 0);
            vmWriter.writePop(Segment::POINTER, 1); // Set THAT
            vmWriter.writePush(Segment::TEMP, 0);
            vmWriter.writePop(Segment::THAT, 0);
        } else {
            VarKind kind = symbolTable.kindOf(varName);
            int index = symbolTable.indexOf(varName);
            vmWriter.writePop(kindToSegment(kind), index);
        }
    }
    
    void compileWhile() {
        int labelNum = getNextLabel();
        std::string L1 = "WHILE_EXP" + std::to_string(labelNum);
        std::string L2 = "WHILE_END" + std::to_string(labelNum);
        
        eat("while");
        
        vmWriter.writeLabel(L1);
        eat("(");
        compileExpression();
        eat(")");
        
        vmWriter.writeArithmetic(Command::NOT);
        vmWriter.writeIf(L2);
        
        eat("{");
        compileStatements();
        eat("}");
        
        vmWriter.writeGoto(L1);
        vmWriter.writeLabel(L2);
    }
    
    void compileReturn() {
        eat("return");
        
        if (tokenizer.getCurrentToken() != ";") {
            compileExpression();
        } else {
            // Void function must return 0
            vmWriter.writePush(Segment::CONST, 0);
        }
        
        eat(";");
        vmWriter.writeReturn();
    }
    
    void compileIf() {
        int labelNum = getNextLabel();
        std::string L1 = "IF_TRUE" + std::to_string(labelNum);
        std::string L2 = "IF_FALSE" + std::to_string(labelNum);
        std::string L3 = "IF_END" + std::to_string(labelNum);
        
        eat("if");
        eat("(");
        compileExpression();
        eat(")");
        
        vmWriter.writeIf(L1);
        vmWriter.writeGoto(L2);
        vmWriter.writeLabel(L1);
        
        eat("{");
        compileStatements();
        eat("}");
        
        if (tokenizer.getCurrentToken() == "else") {
            vmWriter.writeGoto(L3);
            vmWriter.writeLabel(L2);
            
            eat("else");
            eat("{");
            compileStatements();
            eat("}");
            
            vmWriter.writeLabel(L3);
        } else {
            vmWriter.writeLabel(L2);
        }
    }
    
    void compileExpression() {
        compileTerm();
        
        while (isOp()) {
            char op = tokenizer.symbol();
            eat(std::string(1, op));
            compileTerm();
            compileOp(op);
        }
    }
    
    void compileTerm() {
        if (tokenizer.tokenType() == TokenType::INT_CONST) {
            vmWriter.writePush(Segment::CONST, tokenizer.intVal());
            eat(tokenizer.getCurrentToken());
        } else if (tokenizer.tokenType() == TokenType::STRING_CONST) {
            std::string str = tokenizer.stringVal();
            vmWriter.writePush(Segment::CONST, str.length());
            vmWriter.writeCall("String.new", 1);
            for (char c : str) {
                vmWriter.writePush(Segment::CONST, c);
                vmWriter.writeCall("String.appendChar", 2);
            }
            eat(tokenizer.getCurrentToken());
        } else if (isKeywordConstant()) {
            std::string keyword = tokenizer.getCurrentToken();
            if (keyword == "true") {
                vmWriter.writePush(Segment::CONST, 0);
                vmWriter.writeArithmetic(Command::NOT);
            } else if (keyword == "false" || keyword == "null") {
                vmWriter.writePush(Segment::CONST, 0);
            } else if (keyword == "this") {
                vmWriter.writePush(Segment::POINTER, 0);
            }
            eat(keyword);
        } else if (tokenizer.getCurrentToken() == "(") {
            eat("(");
            compileExpression();
            eat(")");
        } else if (isUnaryOp()) {
            char op = tokenizer.symbol();
            eat(std::string(1, op));
            compileTerm();
            if (op == '-') {
                vmWriter.writeArithmetic(Command::NEG);
            } else if (op == '~') {
                vmWriter.writeArithmetic(Command::NOT);
            }
        } else {
            // varName, varName[expression], or subroutineCall
            std::string name = tokenizer.getCurrentToken();
            eatIdentifier();
            
            if (tokenizer.getCurrentToken() == "[") {
                // Array access
                eat("[");
                VarKind kind = symbolTable.kindOf(name);
                int index = symbolTable.indexOf(name);
                vmWriter.writePush(kindToSegment(kind), index);
                compileExpression();
                vmWriter.writeArithmetic(Command::ADD);
                vmWriter.writePop(Segment::POINTER, 1); // Set THAT
                vmWriter.writePush(Segment::THAT, 0);
                eat("]");
            } else if (tokenizer.getCurrentToken() == "(" || tokenizer.getCurrentToken() == ".") {
                // Subroutine call
                int nArgs = 0;
                std::string functionName;
                
                if (tokenizer.getCurrentToken() == ".") {
                    eat(".");
                    std::string methodName = tokenizer.getCurrentToken();
                    eatIdentifier();
                    
                    if (symbolTable.exists(name)) {
                        VarKind kind = symbolTable.kindOf(name);
                        int index = symbolTable.indexOf(name);
                        vmWriter.writePush(kindToSegment(kind), index);
                        functionName = symbolTable.typeOf(name) + "." + methodName;
                        nArgs = 1;
                    } else {
                        functionName = name + "." + methodName;
                    }
                } else {
                    vmWriter.writePush(Segment::POINTER, 0);
                    functionName = className + "." + name;
                    nArgs = 1;
                }
                
                eat("(");
                nArgs += compileExpressionList();
                eat(")");
                
                vmWriter.writeCall(functionName, nArgs);
            } else {
                // Simple variable
                VarKind kind = symbolTable.kindOf(name);
                int index = symbolTable.indexOf(name);
                vmWriter.writePush(kindToSegment(kind), index);
            }
        }
    }
    
    int compileExpressionList() {
        int nArgs = 0;
        
        if (tokenizer.getCurrentToken() != ")") {
            compileExpression();
            nArgs++;
            
            while (tokenizer.getCurrentToken() == ",") {
                eat(",");
                compileExpression();
                nArgs++;
            }
        }
        
        return nArgs;
    }
};

#endif // COMPILATION_ENGINE_H
