#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum class VarKind {
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};

struct Symbol {
    std::string type;
    VarKind kind;
    int index;
    
    Symbol() : type(""), kind(VarKind::NONE), index(0) {}
    Symbol(const std::string& t, VarKind k, int i) 
        : type(t), kind(k), index(i) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> classTable;
    std::unordered_map<std::string, Symbol> subroutineTable;
    int staticIndex;
    int fieldIndex;
    int argIndex;
    int varIndex;

public:
    SymbolTable() : staticIndex(0), fieldIndex(0), argIndex(0), varIndex(0) {}
    
    // Starts a new subroutine scope (resets the subroutine symbol table)
    void startSubroutine() {
        subroutineTable.clear();
        argIndex = 0;
        varIndex = 0;
    }
    
    // Defines a new identifier
    void define(const std::string& name, const std::string& type, VarKind kind) {
        switch (kind) {
            case VarKind::STATIC:
                classTable[name] = Symbol(type, kind, staticIndex++);
                break;
            case VarKind::FIELD:
                classTable[name] = Symbol(type, kind, fieldIndex++);
                break;
            case VarKind::ARG:
                subroutineTable[name] = Symbol(type, kind, argIndex++);
                break;
            case VarKind::VAR:
                subroutineTable[name] = Symbol(type, kind, varIndex++);
                break;
            case VarKind::NONE:
                break;
        }
    }
    
    // Returns the number of variables of the given kind already defined in the current scope
    int varCount(VarKind kind) const {
        switch (kind) {
            case VarKind::STATIC: return staticIndex;
            case VarKind::FIELD: return fieldIndex;
            case VarKind::ARG: return argIndex;
            case VarKind::VAR: return varIndex;
            default: return 0;
        }
    }
    
    // Returns the kind of the named identifier in the current scope
    VarKind kindOf(const std::string& name) const {
        if (subroutineTable.count(name)) {
            return subroutineTable.at(name).kind;
        }
        if (classTable.count(name)) {
            return classTable.at(name).kind;
        }
        return VarKind::NONE;
    }
    
    // Returns the type of the named identifier in the current scope
    std::string typeOf(const std::string& name) const {
        if (subroutineTable.count(name)) {
            return subroutineTable.at(name).type;
        }
        if (classTable.count(name)) {
            return classTable.at(name).type;
        }
        return "";
    }
    
    // Returns the index assigned to the named identifier
    int indexOf(const std::string& name) const {
        if (subroutineTable.count(name)) {
            return subroutineTable.at(name).index;
        }
        if (classTable.count(name)) {
            return classTable.at(name).index;
        }
        return -1;
    }
    
    // Check if identifier exists
    bool exists(const std::string& name) const {
        return subroutineTable.count(name) || classTable.count(name);
    }
    
    // Reset class-level table
    void reset() {
        classTable.clear();
        subroutineTable.clear();
        staticIndex = 0;
        fieldIndex = 0;
        argIndex = 0;
        varIndex = 0;
    }
};

#endif // SYMBOL_TABLE_H
