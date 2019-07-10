/* File: statement.h
 * Authors: Elias
 */

#ifndef STATEMENT_H
#define STATEMENT_H

#include <list>

#include "token.h"
#include "expression.h"

// Abstract class for implementing a visitor class for the Statement class
class StatementVisitor;

// Abstract class for implementing other statements
class Statement {
public:
    virtual void accept(StatementVisitor *visitor);
    virtual ~Statement();
};

// Holds a block statement, which is essentially just a list of statements
class Block : public Statement {
public:
    std::list<Statement *> body;

    virtual void accept(StatementVisitor *visitor) override;
    ~Block();
    Block(std::list<Statement *> body);
};

// Holds a function prototype of the form
// type funName(arglist);
class FunctionDec : public Statement {
public:
    Token returnType;
    Token name;
    // a list of (type, id) pairs
    std::list<std::pair<Token, Token>> arglist;

    virtual void accept(StatementVisitor *visitor) override;
    ~FunctionDec();
    FunctionDec(Token returnType, Token name, std::list<std::pair<Token, Token>> arglist);
};

// Holds a function definition of the form
// type funName(arglist) {statements}
class FunctionDef : public Statement {
public:
    Statement *fDec;
    Statement *body;

    virtual void accept(StatementVisitor *visitor) override;
    ~FunctionDef();
    FunctionDef(Statement *fDec, Statement *body);
};

// Holds a variable declaration with an initializing expression
class VarDec : public Statement {
public:
    Token type;
    Token name;
    Expression *init;
    
    virtual void accept(StatementVisitor *visitor) override;
    ~VarDec();
    VarDec(Token type, Token name, Expression *init);
};

// Holds a return statement
// Holds the return token for error reporting (the token contains the
// line number)
class Return : public Statement {
public:
    Token retTok;
    Expression *retVal;

    virtual void accept(StatementVisitor *visitor) override;
    ~Return();
    Return(Token retTok, Expression *retVal);
};

// Holds an if statement
// if (exp) statement [else statement]
class If : public Statement {
public:
    Expression *condition;
    Statement *body;
    Statement *elseBranch;

    virtual void accept(StatementVisitor *visitor);
    If(Expression *condition, Statement *body, Statement *elseBranch);
    ~If();
};

// Holds a for loop statement
// for (expStmt | vardec; condExp; incExp) stmt
// Note that a 'for' statement does not need any statements/expressions
// in the parenthesis and they can just be null
class For : public Statement {
public:
    Statement *init;
    Expression *condition;
    Expression *inc;
    Statement *body;

    virtual void accept(StatementVisitor *visitor) override;

    For(Statement *init, Expression *condition, Expression *inc, Statement *body);
    ~For();
};

// Holds a while loop statement
// while (exp) stmt
class While : public Statement {
public:
    Expression *condition;
    Statement *body;

    virtual void accept(StatementVisitor *visitor) override;
    ~While();
    While(Expression *condition, Statement *body);
};

// Holds a switch statement
// switch (exp) {caseStmt*}
class Switch : public Statement {
public:
    Expression *value;
    std::list<Statement *> body;

    virtual void accept(StatementVisitor *visitor) override;
    ~Switch();
    Switch(Expression *value, std::list<Statement *> body);
};

// Holds a case statement
// case exp: stmt*
class Case : public Statement {
public:
    Expression *value;
    std::list<Statement *> body;

    virtual void accept(StatementVisitor *visitor) override;
    ~Case();
    Case(Expression *value, std::list<Statement *> body);
};

// Holds a label
// ID:
class Label : public Statement {
public:
    Token id;

    virtual void accept(StatementVisitor *visitor) override;
    ~Label();
    Label(Token id);
};

// Holds a goto statement
// goto ID;
class Goto : public Statement {
public:
    Token id;

    virtual void accept(StatementVisitor *visitor) override;
    ~Goto();
    Goto(Token id);
};

// Holds a break statement
// break;
class Break : public Statement {
public:
    Token breakTok;

    virtual void accept(StatementVisitor *visitor) override;
    ~Break();
    Break(Token breakTok);
};

// Holds an expression statement
// for example: printf("Hello, World!\n");
class ExpStat : public Statement {
public:
    Expression *exp;

    virtual void accept(StatementVisitor *visitor) override;
    ~ExpStat();
    ExpStat(Expression *exp);
};

// Defines what a Statement visitor needs to implement
// The functions are purely virtual so you can't instantiate it
class StatementVisitor {
public:
    virtual void visit(Statement &stat) = 0;
    virtual void visit(FunctionDec &stat) = 0;
    virtual void visit(FunctionDef &stat) = 0;
    virtual void visit(VarDec &stat) = 0;
    virtual void visit(Block &stat) = 0;
    virtual void visit(Return &stat) = 0;
    virtual void visit(If &stat) = 0;
    virtual void visit(For &stat) = 0;
    virtual void visit(While &stat) = 0;
    virtual void visit(Switch &stat) = 0;
    virtual void visit(Case &stat) = 0;
    virtual void visit(Label &stat) = 0;
    virtual void visit(Goto &stat) = 0;
    virtual void visit(Break &stat) = 0;
    virtual void visit(ExpStat &stat) = 0;
};

#endif
