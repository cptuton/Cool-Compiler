/* File: parser.h
 * Authors: Nathan / Michael
 */
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <list>
#include <map>
#include <vector>

#include "token.h"
#include "statement.h"
#include "expression.h"
#include "exceptions.h"
#include <iostream>
#include <utility> 

class Parser {
    private:
        std::vector<Token> tokens;
        // Keep track of the token we are currently looking at
        int current;
        bool err;
        Token nextToken();

        std::list<Statement *> program();
        std::list<Statement *> decList();
        Statement *declaration();
        Statement *function();
        std::list<std::pair<Token, Token>> params();
        std::list<std::pair<Token, Token>> paramList();
        std::list<Statement *> statementList();
        
        Statement *varDecl();
        //Statement *varDecList();
        //Statement *varDecInit();
        //Statement *funProto();
        Statement *funcDecl();
        Statement *block();
        Statement *returnStmt();
        Statement *statement();
        Statement *whileStmt();
        Statement *forStmt();
        Statement *ifStmt();
        Statement *switchStmt();
        Statement *expressionStmt();

        std::list<Statement *> caseList();
        Statement *caseStmt();
        Statement *gotoStmt();
        Statement *label();

        Token constant();
        Token typeSpecifier();
        Token returnType();

        // Helper functions
        // Return the current token and advance to the next one
        Token advance();
        // Peek at the previously consumed token
        Token previous();
        // Peek at the current token without consuming it
        Token peek();
        // Match a list of tokens to match agains the current
        bool match(std::list<TokenType> tokenList);
        // Check if the current token is 'type'
        bool check(TokenType type);
        // Checks if we hit the end of the token list
        bool isAtEnd();
        // Checks if the current token is TYPE_*
        bool isType();
        // Check if the next token is of type 'type'
        bool checkNext(TokenType type);
        // Synchronize the token list
        void synchronize();

        // Expression *Functions
        Expression *expression();
        Expression *expressionList();        
        
        Expression *simpleExpression();
        Expression *andExpr();
        Expression *bitOrExpr();
        Expression *bitXorExpr();
        Expression *bitAndExpr();
        Expression *relop1Expr();
        Expression *relop2Expr();
        Expression *bitShiftExpr();
        Expression *sumExpr();
        Expression *term();
        Expression *preUnaryExpr();
        Expression *postUnaryExpr();
        Expression *factor();
        Expression *immutable();
        Expression *call();
        std::list<Expression *> args();

    public:
        bool hadError();
        Parser(std::vector<Token> tokens);
        std::list<Statement *> parse();
};

    void freeAST(std::list<Statement *> &ast);
#endif
