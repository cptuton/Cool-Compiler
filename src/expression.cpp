/* File: expression.cpp
 * Authors: Elias
 * Description: Implement the base Expression class and its children classes
 *              to represent nodes in the AST
 */

#include <iostream>

#include "expression.h"

//---------------------------------------
// Expression

void Expression::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

Expression::~Expression(){
}

//---------------------------------------
// Binary Expression

BinaryExp::BinaryExp(Expression *left, Token op, Expression *right):
    op(op) {
    this->left = left;
    this->right = right;
}

void BinaryExp::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

BinaryExp::~BinaryExp() {
    if (left) delete left;
    if (right) delete right;
}

//---------------------------------------
// Unary Expression

UnaryExp::UnaryExp(Token op, Expression *exp):
    op(op) {
    this->op = Token(op);
    this->exp = exp;
}

void UnaryExp::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

UnaryExp::~UnaryExp() {
    if (exp) delete exp;
}

//---------------------------------------
// Primary Expression

PrimaryExp::PrimaryExp(Token value):
    value(value) {
    this->value = Token(value);
}

void PrimaryExp::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

PrimaryExp::~PrimaryExp() {
}

//---------------------------------------
// Grouping Expression

GroupExp::GroupExp(Expression *exp) {
    this->exp = exp;
}

void GroupExp::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

GroupExp::~GroupExp() {
    if (exp) delete exp;
}

//---------------------------------------
// FunCall Expression

FunCall::FunCall(Token name, std::list<Expression *> arglist):
    name(name) {
    this->arglist = arglist;
}

void FunCall::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

FunCall::~FunCall() {
for(auto expr:arglist) delete expr;
}

//---------------------------------------
// Assignment Expression

Assignment::Assignment(Token name, Token op, Expression *exp):
    name(name), op(op)
{
    this->exp = exp;
}

void Assignment::accept(ExpressionVisitor *visitor) {
    visitor->visit(*this);
}

Assignment::~Assignment() {
    if (exp) delete exp;
}
