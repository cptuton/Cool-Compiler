/** File: irepvisit.cpp
 *  Author: Nathan
 *  Description: Header file for parsetree too IR visitor pattern
 */
 
 /* File: pprinter.h
 * Authors: Nathan Nguyen
 */

 
#ifndef IREPVISIT_H
#define IREPVISIT_H

#include "expression.h"
#include "statement.h"
#include "irep.h"
#include "symbolvisit.h"

// Need a more concrete class to store every type of instructions
// Printer class that inherits from the expression and statement visitor patterns
class IRepVisitor : public ExpressionVisitor , public  StatementVisitor {
private:
    // Marks the location of every goto statement jump instruction in the instruction list
    // As well as what function it belongs too, same for every label instruction
    std::list<int> goToMarker;
    std::vector<std::string> goToFuncName;
    std::vector<std::string> goToLabelName;
    std::vector<std::string> labelNames;
    std::vector<std::string> labelFunctionNames;

    //holds last used function name, 
    //used to mark the end of function bodies
    std::string funNameHolder;
    // Instructions that are created from visiting the parse tree
    std::list<Instruction *> irep;
    
    // Current Register Number updated as let instr are called
    int currentReg = 0;
    int currentLabel = 0;
    
    // Symbol Table with scope level to know which of the functions we are in
    // 0 is the global
    SymbolTable* symbolTable;
    int scope = 0;
    int inFunc = 0;
    
public:
    // StatementVisitor virtual methods to override
    virtual void visit(Statement &stat);
    virtual void visit(FunctionDec &stat);
    virtual void visit(FunctionDef &stat);
    virtual void visit(VarDec &stat);
    // Block 
    virtual void visit(Block &stat);
    virtual void visit(Return &stat);
    virtual void visit(While &stat);
    virtual void visit(ExpStat &stat);
    virtual void visit(If &stat);
    virtual void visit(For &stat);
    virtual void visit(Switch &stat);
    virtual void visit(Case &stat);
    virtual void visit(Label &stat);
    virtual void visit(Goto &stat);
    virtual void visit(Break &stat);

    // ExpresisonVisitor virtual methods to override
    virtual void visit(Expression &exp);
    virtual void visit(BinaryExp &exp);
    virtual void visit(UnaryExp &exp);
    virtual void visit(PrimaryExp &exp);
    virtual void visit(GroupExp &exp);
    virtual void visit(FunCall &exp);
    virtual void visit(Assignment &exp);
    // Printing function
    void printIR();
    
    // Generate sequential number of register names 
    int generateReg();
    int getReg();
    int generateLabel();
    std::list<Instruction *> getIR();
    // Constructor to take in symbol Table
    IRepVisitor(SymbolTable *table); 
    IRepVisitor(); 

    // Helper
    SymbolType getSymType(Token t);
    
    // Label Checker
    bool labelsAreGood();
};

#endif
