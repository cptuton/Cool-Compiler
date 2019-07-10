/* File: symbol.h
 * Authors: Christian
 * Description: Header file for symbol.cpp. Defines structure for symbol table
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "token.h"
#include "c_type.h"

// Structure for holding symbol values of variables and functions
class Symbol {
private:
    SymbolType type;
    SymbolType returnValue;
    std::vector<SymbolType> parameters;
public:
    Symbol(SymbolType type, SymbolType returnValue,
           std::vector<SymbolType> parameters);
    Symbol(SymbolType type);

    SymbolType getType();
    SymbolType getReturn();
    std::vector<SymbolType> getParameters();
    bool isVariable();
    bool isFunction();
    bool hasProto;
    bool hasDef;
    friend std::ostream& operator << (std::ostream &out, Symbol &symbol);
};

// Defines symbol table structure
class SymbolTable {
    private:
        std::map<std::string, Symbol> varTable;
        std::map<std::string, Symbol> funTable;

        SymbolTable *parent; // Pointer to next scope up
        int level;           // Current scope
        std::vector<SymbolTable> internalScopes; // Internal blocks
    public:
        std::string nextContext;
        SymbolTable(SymbolTable *parent, std::string functionCtxt);
        std::string functionCtxt; // Name of parent function, empty if global
        void insertVar(std::string key, Symbol value);
        void insertFun(std::string key, Symbol value);
        Symbol *lookupVar(std::string key);
        Symbol *lookupFun(std::string key);
        Symbol *localVarLookup(std::string key);
        Symbol *localFunLookup(std::string key);
        int currentLevel();
        void printVarTable();
        void printFunTable();
        SymbolTable *addScope();
        SymbolTable *getParent();
};

std::string stringOfSymbolType(SymbolType type);
SymbolType symbolOfType(Token t);
// Test function
void symTest();

#endif
