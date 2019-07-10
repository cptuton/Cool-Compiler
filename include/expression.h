/* File: expression.h
 * Authors: Elias
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <list>
#include "token.h"

// Abstract class for implementing a visitor class for the Expression class
class ExpressionVisitor;

// Abstract class for implementing other expressions
class Expression {
public:
    virtual void accept(ExpressionVisitor *visitor);
    virtual ~Expression();
};

// Holds a binary expression with two operands
class BinaryExp : public Expression {
public:
    Expression *left;
    Expression *right;
    Token op;

    void accept(ExpressionVisitor *visitor);

    BinaryExp(Expression *left, Token op, Expression *right);
    ~BinaryExp();
};

// Holds a unary expression with one operand
class UnaryExp : public Expression {
public:
    Expression *exp;
    Token op;

    void accept(ExpressionVisitor *visitor);

    UnaryExp(Token op, Expression *exp);
    ~UnaryExp();
};

// Holds a constant value or id
class PrimaryExp : public Expression {
public:
    Token value;

    void accept(ExpressionVisitor *visitor);

    PrimaryExp(Token value);
    ~PrimaryExp();
};

// Internal representation of ( exp )
class GroupExp : public Expression {
public:
    Expression *exp;

    void accept(ExpressionVisitor *visitor);

    GroupExp(Expression *exp);
    ~GroupExp();
};

// Holds a function call
class FunCall : public Expression {
public:
    Token name;
    std::list<Expression *> arglist;

    void accept(ExpressionVisitor *visitor);

    FunCall(Token name, std::list<Expression *> arglist);
    ~FunCall();
};

// Holds an assignment
// id = exp
class Assignment : public Expression {
public:
    Token name;
    Token op;
    Expression *exp;
    void accept(ExpressionVisitor *visitor);

    Assignment(Token name, Token op, Expression *exp);
    ~Assignment();
};

// Defines what an Expression visitor needs to implement
// The functions are purely virtual so you can't instantiate it
class ExpressionVisitor {
public:
    virtual void visit(Expression &exp) = 0;
    virtual void visit(BinaryExp &exp) = 0;
    virtual void visit(UnaryExp &exp) = 0;
    virtual void visit(PrimaryExp &exp) = 0;
    virtual void visit(GroupExp &exp) = 0;
    virtual void visit(FunCall &exp) = 0;
    virtual void visit(Assignment &exp) = 0;
};

#endif
