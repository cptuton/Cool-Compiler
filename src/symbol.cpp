/** File: symbol.cpp
 *  Authors: Christian
 *  Description: Implementation of symbol table
 */

#include "symbol.h"
#include "symbolvisit.h"

// Constructor for function symbol
Symbol::Symbol(SymbolType type, SymbolType returnValue,
        std::vector<SymbolType> parameters) {
    this->type = type;
    this->returnValue = returnValue;
    this->parameters = parameters;
    hasProto = false;
    hasDef = false;
}

// Constructor for variable symbol
Symbol::Symbol(SymbolType type) {
    this->type = type;
    hasProto = false;
    hasDef = false;
}

// Returns symbol type
SymbolType Symbol::getType() {
    return this->type;
}

// Returns return value (functions only)
SymbolType Symbol::getReturn() {
    return this->returnValue;
}

// Returns pointer to symbol list from parameters (functions only)
std::vector<SymbolType> Symbol::getParameters() {
    return this->parameters;
}

// Helper functions ---------------------------------------
bool Symbol::isVariable() {
    if ((this->type == SYM_FUNCT) || (this->type == SYM_VOID)) return false;
    return true;
}

bool Symbol::isFunction() {
    if (this->type == SYM_FUNCT) return true;
    return false;
}

// Overload the '<<' operator
// Lets you put symbol object in a std::ostream expression
std::ostream& operator << (std::ostream &out, Symbol &symbol) {
    // Function symbol
    if (symbol.isFunction()) {
        out << stringOfSymbolType(symbol.getReturn()) << " ( ";
        for (auto s: symbol.getParameters()) {
            out << stringOfSymbolType(s) << ", ";
        }
        return out << ")";
    }
    // Variable symbol
    return out << stringOfSymbolType(symbol.getType());
}

// Return a string representation of symbol type
std::string stringOfSymbolType(SymbolType type) {
    switch (type) {
        case SYM_INT: return "INT";
        case SYM_CHAR: return "CHAR";
        case SYM_FLOAT: return "FLOAT";
        case SYM_VOID: return "VOID";
        case SYM_FUNCT: return "FUNCT";
        case SYM_ERR: return "ERR";
    }
    return "ERROR";
}

// Returns a symbol enum based on input token.
SymbolType symbolOfType(Token t) {
    switch (t.getType()) {
        case TYPE_INT: return SYM_INT;
        case TYPE_CHAR: return SYM_CHAR;
        case TYPE_FLOAT: return SYM_FLOAT;
        case TYPE_VOID: return SYM_VOID;
        default: return SYM_ERR;
    }
}

// --------------------------------------------------------

// Symbol table constructor, currentTable is symbol table of current scope.
// NULL if currently in global scope
SymbolTable::SymbolTable(SymbolTable *parent, std::string functionCtxt) {
    this->parent = parent;
    this->functionCtxt = functionCtxt;
    if (parent == NULL) {
        this->level = 0;
    } else {
        this->level = 1 + parent->level;
    }
}

// Insert variable into symbol table
void SymbolTable::insertVar(std::string key, Symbol value) {
    this->varTable.insert(std::pair<std::string, Symbol>(key, value));
}

// Insert function into symbol table
void SymbolTable::insertFun(std::string key, Symbol value) {
    this->funTable.insert(std::pair<std::string, Symbol>(key, value));
}

// Recursively lookup identifier in symbol table chain. NULL if doesn't exist
Symbol* SymbolTable::lookupVar(std::string key) {
    std::map<std::string, Symbol>::iterator it;
    it = this->varTable.find(key);
    if (it != this->varTable.end())
        return &(it->second);
    if (this->parent == NULL)
        return NULL;
    return this->parent->lookupVar(key);
}

Symbol* SymbolTable::lookupFun(std::string key) {
    std::map<std::string, Symbol>::iterator it;
    it = this->funTable.find(key);
    if (it != this->funTable.end())
        return &(it->second);
    if (this->parent == NULL)
        return NULL;
    return this->parent->lookupFun(key);
}

// lookup identifier in symbol table at current scope. NULL if doesn't exist
Symbol* SymbolTable::localVarLookup(std::string key) {
    std::map<std::string, Symbol>::iterator it;
    it = this->varTable.find(key);
    if (it != this->varTable.end())
        return &(it->second);
    return NULL;
}

Symbol* SymbolTable::localFunLookup(std::string key) {
    std::map<std::string, Symbol>::iterator it;
    it = this->funTable.find(key);
    if (it != this->funTable.end())
        return &(it->second);
    return NULL;
}


// Returns current level (scope) of symbol table
int SymbolTable::currentLevel() {
    return this->level;
}

// Prints all symbol tables recursively
void SymbolTable::printVarTable() {
    std::cout << "Table (level " << this->currentLevel() << ")\n---------------\n";
    for (std::map<std::string, Symbol>::iterator it=varTable.begin();
            it != varTable.end(); it++) {
        std::cout << it->first << "      " << it->second << std::endl;
    }

    for (auto i : internalScopes) {
        i.printVarTable();
    }

}

void SymbolTable::printFunTable() {
    std::cout << "Table (level " << this->currentLevel() << ")\n---------------\n";
    for (std::map<std::string, Symbol>::iterator it=funTable.begin();
            it != funTable.end(); it++) {
        std::cout << it->first << "      " << it->second << std::endl;
    }

    for (auto i : internalScopes) {
        i.printFunTable();
    }

}


// Symboltable test
void symTest() {
    SymbolTable global(NULL, "");


    /* Create symbol tables
    SymbolTable symtbl(NULL);
    SymbolTable nested(&symtbl);
    
    // Create symbols to be added to table
    Symbol s1 (SYM_INT);
    Symbol s2 (SYM_CHAR);
    std::vector<SymbolType> param1;
    std::vector<SymbolType> param2;
    
    // Populate function parameters
    param1.push_back(SYM_INT);
    param1.push_back(SYM_FLOAT);
    param1.push_back(SYM_CHAR);
    Symbol f1 (SYM_FUNCT, SYM_INT, param1);
    Symbol f2 (SYM_FUNCT, SYM_VOID, param2);
    
    // Populate symbol table
    symtbl.insert("x", s1);
    symtbl.insert("someFunction", f1);
    nested.insert("scopedFunction", f2);
    nested.insert("y", s2);

    // Print symbols
    std::cout << s1 << std::endl;
    std::cout << s2 << std::endl;
    std::cout << f1 << std::endl;
    std::cout << f2 << std::endl;

    // Print symbol tables
    symtbl.print();
    std::cout << std::endl;
    nested.print();
    
    // Lookup
    Symbol *find;
    find = nested.lookup("x");
    if (find != NULL)
        std::cout << std::endl << *find << std::endl;
    else
        std::cout << std::endl << "Could not be found" << std::endl;
    */
}

SymbolTable *SymbolTable::addScope() {
    internalScopes.push_back(SymbolTable(this, this->nextContext));
    return &internalScopes.back();
}

SymbolTable *SymbolTable::getParent() {
    return parent;
}
