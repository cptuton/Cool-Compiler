/* File: statement.cpp
 * Authors: Elias
 * Description: Implement the base Statement class and its children classes
 *              to represent nodes in the AST
 */

#include "statement.h"

//---------------------------------------
// Statement

void Statement::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Statement::~Statement() {
}

//---------------------------------------
// Function Declaration Statement

FunctionDec::FunctionDec(Token returnType, Token name, std::list<std::pair<Token, Token>> arglist):
    returnType(returnType), name(name) {
    this->arglist = arglist;
}

void FunctionDec::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

FunctionDec::~FunctionDec() {
}

//---------------------------------------
// Function Definition Statement

FunctionDef::FunctionDef(Statement *fDec, Statement *body):
    fDec(fDec), body(body)
{
}

void FunctionDef::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

FunctionDef::~FunctionDef() {
    if (fDec) delete fDec;
    if (body) delete body;
}

//---------------------------------------
// VarDec Statement

VarDec::VarDec(Token type, Token name, Expression *init):
    type(type), name(name) {
    this->init = init;
}

void VarDec::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

VarDec::~VarDec() {
    if (init) delete init;
}

//---------------------------------------
// Block Statement

Block::Block(std::list<Statement *> body) {
    this->body = body;
}

void Block::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Block::~Block() {
for(auto stmt:body) delete stmt;
}
//---------------------------------------
// Return Statement

Return::Return(Token retTok, Expression *retVal):
    retTok(retTok) {
    this->retVal = retVal;
}

void Return::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Return::~Return() {
    if (retVal) delete retVal;
}

//---------------------------------------
// If Statement

If::If(Expression *condition, Statement *body, Statement *elseBranch) {
    this->condition = condition;
    this->body = body;
    this->elseBranch = elseBranch;
}

void If::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

If::~If() {
    if (condition)
        delete condition;
    if (body)
        delete body;
    if (elseBranch)
        delete elseBranch;
}

//---------------------------------------
// For loop Statement

For::For(Statement *init, Expression *condition, Expression *inc, Statement *body) {
    this->init = init;
    this->condition = condition;
    this->inc = inc;
    this->body = body;
}

void For::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

For::~For() {
    if (init)
        delete init;
    if (condition)
        delete condition;
    if (body)
        delete body;
}

//---------------------------------------
// While loop Statement

While::While(Expression *condition, Statement *body) {
    this->condition = condition;
    this->body = body;
}

void While::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

While::~While() {
    if (condition) delete condition;
    if (body) delete body;
}

//---------------------------------------
// Switch Statement

Switch::Switch(Expression *value, std::list<Statement *> body) {
    this->value = value;
    this->body = body;
}

void Switch::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Switch::~Switch() {
    if (value) delete value;
    for (Statement *s : body) delete s;
}

//---------------------------------------
// Case

Case::Case(Expression *value, std::list<Statement *>body) {
    this->value = value;
    this->body = body;
}

void Case::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Case::~Case() {
    if (value) delete value;
    for (Statement *s : body) delete s;
}

//---------------------------------------
// Label

Label::Label(Token id):
    id(id)
{
}

void Label::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Label::~Label() {
}

//---------------------------------------
// Goto Statement

Goto::Goto(Token id):
    id(id)
{
}

void Goto::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Goto::~Goto(){
}

//---------------------------------------
// Break Statement

Break::Break(Token breakTok):
    breakTok(breakTok)
{
}

void Break::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

Break::~Break() {
}

//---------------------------------------
// Expression Statement

ExpStat::ExpStat(Expression *exp) {
    this->exp = exp;
}

void ExpStat::accept(StatementVisitor *visitor) {
    visitor->visit(*this);
}

ExpStat::~ExpStat() {
    if (exp) delete exp;
}
