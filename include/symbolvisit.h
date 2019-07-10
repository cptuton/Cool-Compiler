/* File: symbolvisit.h
 * Authors: Christian
 */

#ifndef SYMBOLVISIT_H
#define SYMBOLVISIT_H

#include "expression.h"
#include "statement.h"
#include "symbol.h"
#include "exceptions.h"

// Visitor pattern class
class SymbolVisit : public ExpressionVisitor, public StatementVisitor {
private:
    // std::vector<SymbolTable> tableList;
    SymbolTable *global;
    SymbolTable *current;
    bool visitingDef = false;
    bool visitingFunct = false;
    std::list<std::pair<Token, Token>> funVars;

public:
    // StatementVistor virtual methods to override
    virtual void visit(Statement &stat);
    virtual void visit(FunctionDec &stat);
    virtual void visit(FunctionDef &stat);
    virtual void visit(VarDec &stat);
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

    // Symbol table getters
    // std::vector<SymbolTable> getTableList();
    SymbolTable *getCurrent();

    // Full table list print
    void printTableList();

    SymbolTable *getGlobal();

    // Constructor
    SymbolVisit();

    bool thrownErr = false;
};

#endif
