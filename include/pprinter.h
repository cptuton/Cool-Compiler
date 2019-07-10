/* File: pprinter.h
 * Authors: Nathan Nguyen
 */

 
#ifndef PPRINTER_H
#define PPRINTER_H

#include "expression.h"
#include "statement.h"

// Printer class that inherits from the expression and statement visitor patterns
class PrettyPrinter : public ExpressionVisitor , public  StatementVisitor {
private:
    int level = 0;
public:
    // StatementVisitor virtual methods to override
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

    // Pretty printing indentation helper
    void indent();
};

#endif
