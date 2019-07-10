/* File: parser.cpp
 * Authors: Nathan / Michael
 * Description:
 */

#include "parser.h"
#include "pprinter.h"

Parser::Parser(std::vector<Token> tokens) {
    this->tokens = tokens;
    this->current = 0;
    this->err = false;
}
bool Parser::hadError() {
    return this->err;
}


// Checks current token type against passed list of token types
// Advances if found
bool Parser::match(std::list<TokenType> tokenList) {
    for (std::list<TokenType>::iterator i = tokenList.begin(); i != tokenList.end(); ++i) {
        if (check(*i)) {
            advance();
            return true;
        }
    }

    return false;
}

// If not at he end of the list advance the currrent token
Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }

    return previous();
}

// Returns the last token
Token Parser::previous() {
    return tokens.at(current - 1);
}

// Returns a reference to the current token
Token Parser::peek() {
    //std::cout << tokens.at(current) << std::endl;
    return tokens.at(current);
}

// Returns wether the current is at he eof
bool Parser::isAtEnd() {
    return (peek().getType() == EOFILE);
}

// Checks current token type again passed var
bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }

    return (peek().getType() == type);
}

//checks if the next token matches a passed type
bool Parser::checkNext(TokenType type) {
    if (isAtEnd()) {
        return false;
    }

    return (tokens.at(current+1).getType() == type);
}

//checks if the next token is for an identifying type
bool Parser::isType() {
    TokenType type = peek().getType();
    if (type == TYPE_INT || type == TYPE_VOID || type == TYPE_CHAR || type == TYPE_FLOAT) {
        return true;
    }

    return false;
}

// Discard tokens until we reach a semicolon or a left brace
void Parser::synchronize() {
    if (isAtEnd()) {
        throw  ParseError("Error at end of token list", peek());
    } else {
        advance();
    }


    while (!isAtEnd()) {
        if (previous().getType() == SEMICOLON || previous().getType() == LEFT_BRACE)
            return;

        switch (peek().getType()) {
            case IF:
            case ELSE:
            case SWITCH:
            case BREAK:
            case FOR:
            case WHILE:
            case RETURN:
            case GOTO:
            case CONTINUE:
            case RIGHT_BRACE:
                return;
            default:
                ;//continue;
        }

        advance();
    }
}

// Entry function into the parser
std::list<Statement *> Parser::parse() {
    return program();
}

// Function for program production rule, returns decList
std::list<Statement *> Parser::program() {
    try {
        return decList();
    }  catch (ParseError &se) {
        //woooo fixed an error?
        this->err = true;
        return {};
    }
}

// Function for program production rule, returns decList + declaration or declaration
std::list<Statement *> Parser::decList() {
    std::list<Statement *> decs;
    
    // While top != EOF push back declaration

    while(!isAtEnd()) {
        try {
            decs.push_back(declaration());

        } catch (ParseError &se) {
            this->err = true;

            se.printException();

            std::cout << peek().getLexeme() << std::endl;

            synchronize();
        }
    }

    return decs;
}

// Function for program production rule, returns varDec, function, or functionProto
Statement *Parser::declaration() {
    //erro checks for out of bounds
    if((unsigned)current+2 < tokens.size()) {
        //checks if token 2 ahead is a proper type
        if (tokens.at(current+2).getType() == SEMICOLON || tokens.at(current+2).getType() == EQUAL || tokens.at(current+2).getType() == COMMA) {
            return varDecl();
        }
    } else {
        throw ParseError("Expected semicolon.", peek());
    }

    return function();
}

// Function for program production rule, returns funcDecl + block
Statement *Parser::function() {
    Statement *left = funcDecl();

    //if semicolon return func proto
    if (check(SEMICOLON)) {
        advance();
        return left;
    } 
    
    Statement *right = block();

    return new FunctionDef(left, right);
}

// Function for program production rule, returns returnType + id + ( params )
Statement *Parser::funcDecl() {
    
    Token type = returnType();
    
    //error check for identifier
    if (check(IDENTIFIER)) {
        Token id = advance();
        
        //get params 
        std::list<std::pair<Token, Token>> args = params();

       return new FunctionDec(type, id, args);

    } else {
        throw ParseError("Expected identifier", peek());
    }

    //should never reach here
    return nullptr;
}

// Function for program production rule, returns paramList or void
std::list<std::pair<Token, Token>> Parser::params() {
    //check for (
    if (check(LEFT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected left paranthesis.", peek());
    }

    //get list of params
    std::list<std::pair<Token, Token>> args = paramList();
    
    //check for  )
    if (check(RIGHT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected right paranthesis.", peek());
    }

    return args;
}

// Function for program production rule, returns list of
std::list<std::pair<Token, Token>> Parser::paramList() {
    std::list<std::pair<Token, Token>> params;

    //while ) doesn't show up 
    while(!check(RIGHT_PAREN)) {
        Token type = typeSpecifier();
        
        //check for id
        if (check(IDENTIFIER)) {
            Token id = advance(); //consume
            
            //init arg pair
            std::pair<Token, Token> arg(type, id);
            params.push_back(arg);

            // If you find anything other than a comma or right parenthesis //throw exception
            if (!check(COMMA) && !check(RIGHT_PAREN)) {
                throw ParseError("Expected comma or right parenthesis.", peek());
            }

            if (check(COMMA)) {
                advance();
            }
            
        } else {
            throw ParseError("Expected identifier.", peek());
        }
        
    }

    return params;
}

// Function for block production rule, returns { + statement + }
Statement *Parser::block() {
    std::list<Statement *> blocks;
    
    //check for {
    if (check(LEFT_BRACE)) {
        advance();
    } else {
        throw ParseError("Expected left brace.", peek());
    }

    blocks = statementList();

    //check for }
    if (check(RIGHT_BRACE)) {
        advance();
    } else {
        throw ParseError("Expected right brace.", peek());
    }


    return new Block(blocks);
}

//creates a list of statements
std::list<Statement *> Parser::statementList() {
    std::list<Statement *> stmts;
    

    while(!check(RIGHT_BRACE) && !check(CASE) && !check(DEFAULT)) {
        try {
            //pushes statement into list
            stmts.push_back(statement());
        } catch (ParseError &se) { //print error message
            this->err = true;

            se.printException();

            std::cout << peek().getLexeme() << std::endl;

            synchronize();
        }
    }

    return stmts;
}

//function that branches for different statement types
Statement *Parser::statement() {
    if (check(RETURN)) {
        return returnStmt();
    }

    if (check(IF)) {
        return ifStmt();
    }

    if (check(WHILE)) {
        return whileStmt();
    }

    if (check(FOR)) {
        return forStmt();
    }
    

    if (check(LEFT_BRACE)) {
        return block();
    }

    if (check(SWITCH)) {
        return switchStmt();
    }

    if (check(IDENTIFIER)) {
        if (checkNext(COLON)) {
            return label();
        }
    }

    if (check(GOTO)) {
        return gotoStmt();
    }

    if (check(BREAK)) {
        Statement *brk = new Break(advance());

        if (check(SEMICOLON)) {
            advance();
        } else {
            throw ParseError("Expected semicolon.", peek());
        }

        return brk;
    }

    if (isType()) {//if it has a type id check for var dec
        if (tokens.at(current+2).getType() == SEMICOLON || tokens.at(current+2).getType() == EQUAL || tokens.at(current+2).getType() == COMMA) {
            return varDecl();
        }
    } 

    // Defaulting to Expression Statements for the moment
    return expressionStmt();
}

//builds statement for if statement
Statement *Parser::ifStmt() {
    //safe to assume IF exists since it is the entry condition
    advance();

    //check for (
    if (check(LEFT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected left parenthesis.", peek());
    }

    Expression *condition = expressionList();
    //check for)
    if (check(RIGHT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected right parenthesis.", peek());
    }

    Statement *body = statement();
    //check if else exists
    if (check(ELSE)) {
        advance();
        Statement *elseBody = statement();

        return new If(condition, body, elseBody);
    }

    return new If(condition, body, nullptr);
}

// While Statment call
Statement *Parser::whileStmt() {
    //safe to assume WHILE exists since it is the entry condition
    advance();
    
    //check for opening parenthesis
    if (check(LEFT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected left parenthsis.", peek());
    }

    // While not right parand build the expression
    Expression *conditional = expressionList();
    
    // Advance for the ending parenthesis
    if (check(RIGHT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected right parenthsis.", peek());
    }
    
    // The rest is just a block 
    Statement *blockStmt = statement();
    
    return new While(conditional, blockStmt);
}
//build statement for for loop
Statement *Parser::forStmt() {
    //safe to assume for exists since it is the entry condition
    advance();

    //check for opening parenthesis
    if (check(LEFT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected (.", peek());
    }

    Statement *init = expressionStmt();
    //expressionStmt() already checks for ;

    Expression *conditional = expressionList();
    //check for ;
    if (check(SEMICOLON)) {
        advance();
    } else {
        throw ParseError("Expected semicolon.", peek());
    }
    
    Expression *inc = expressionList();

    // Advance for the ending parenthesis
    if (check(RIGHT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected ).", peek());
    }

    Statement *body = statement();

    return new For(init, conditional, inc, body);
}
//builds switch statement
Statement *Parser::switchStmt() {
    //safe to assume switch exists since it is the entry condition
    advance();

    //check for opening parenthesis
    if (check(LEFT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected (.", peek());
    }

    Expression *condition = expressionList();

    // Advance for the ending parenthesis
    if (check(RIGHT_PAREN)) {
        advance();
    } else {
        throw ParseError("Expected ).", peek());
    }

    // Advance for the opening curly brace
    if (check(LEFT_BRACE)) {
        advance();
    } else {
        throw ParseError("Expected {.", peek());
    }

    std::list<Statement *> cases = caseList();

    // Advance for the ending curly brace
    if (check(RIGHT_BRACE)) {
        advance();
    } else {
        throw ParseError("Expected }.", peek());
    }

    return new Switch(condition, cases);
}

//builds and error checks the list of cases for switch body
std::list<Statement *> Parser::caseList() {
    std::list<Statement *> cases;


    while(!check(RIGHT_BRACE)) {
        //check for case and default
        if(check(CASE) || check(DEFAULT)) {
            cases.push_back(caseStmt());
        } else {
            throw ParseError("Expected case or default.", peek());
        }
    }

    return cases;
}

//builds a case statement
Statement *Parser::caseStmt() {
    int caseFlag = 0;

    //make sure top isnt default
    if (check(CASE)) {
        caseFlag = 1;
    }

    //remove case label
    advance();

    if (caseFlag) {
        Expression *label = expression();
        //check for:
        if (check(COLON)) {
            advance();
        } else {
            throw ParseError("Expected colon.", peek());
        }

        std::list<Statement *> body = statementList();
    
        return new Case(label, body);

    } else {
        //check for:
        if (check(COLON)) {
            advance();
        } else {
            throw ParseError("Expected colon.", peek());
        }

        std::list<Statement *> body = statementList();

        return new Case(nullptr, body);
    }
}

//builds gotostatement
Statement *Parser::gotoStmt() {
    //safe to assume goto exists since it is the entry condition
    advance();
    //check for id
    if (check(IDENTIFIER)) {
        Token id = advance();
        //check for ;
        if (check(SEMICOLON)) {
            advance();    
        } else {
            throw ParseError("Expected semicolon.", peek());
        }

        return new Goto(id);
    } else {
        throw ParseError("Expected identifier.", peek());
    }

    //shouldnt ever get here
    return nullptr;
}

//builds a label statement
Statement *Parser::label() {
    //safe to assume name exists since it is the entry condition
    Token id = advance();

    //colon is part of the entry condition
    advance();
    
    // Check to see if the next token is not a part of statement
    if (check(RIGHT_BRACE)) {
        throw ParseError("Label at end of Compound Statement", peek());
    } else if (check(TYPE_INT) || check(TYPE_CHAR) || check(TYPE_FLOAT) || check(TYPE_STRING) || check(TYPE_VOID)) {
        throw ParseError("Label Can't Be followed by declaration", peek());
    } 
    return new Label(id);
}

// Expression statement rule
Statement *Parser::expressionStmt() {
    // Check if the operator is in line with the grammar for expression statement rules 
    Statement *expr = new ExpStat(expressionList());
    //check for;
    if (check(SEMICOLON)) {
        advance();
        return expr;
    } else {
        throw ParseError("Expected semicolon.", peek());
    }
    
    ///return nullptr;
}

// Function For Variable Declarations
Statement *Parser::varDecl() {
    // At this stage it is confirmed that the third token is either a ; or , or = 
    if (isType()) {
        Token type = typeSpecifier();
        //check for id
        if (check(IDENTIFIER)) {
            Token name = advance();

            // Check if Semicolon
            if (check(SEMICOLON)) {
                // Base Case
                Statement *first = new VarDec(type, name, nullptr);
                advance();

                return first;
            }
            //check for =
            if (check(EQUAL)) {
                // Captures the integer output and builds the expression accounting for the semi colon and = character
                advance();

                Expression *expr = expression();

                // Check for semi colon
                if (check(SEMICOLON)) {
                    advance();
                } else {
                    throw ParseError("Expected semicolon.", peek());
                }

                // Expressions go until semi colon anyway so just need to call the expression() function
                return new VarDec(type, name, expr);
            } else {
                throw ParseError("Expected = or semicolon.", peek());
            }

        } else {
            throw ParseError("Expected identifier.", peek());
        }
    } else {
        throw ParseError("Expected type.", peek());
    }

    //should never reach here
    return nullptr;
}

// Function for returnStmt production rule, returns  return + expressionStmt
Statement *Parser::returnStmt() {
    Token ret = advance();

    if (check(SEMICOLON)) {
        advance();
        return new Return(ret, NULL);
    } 

    Expression *con = expressionList();
    //check for ;
    if (check(SEMICOLON)) {
        advance();
    } else {
        throw ParseError("Expected semicolon.", peek());
    }

    return new Return(ret, con);
}

// Function for returnType production rule, returns typeSpecifier or returns void
Token Parser::returnType() {
    if (check(TYPE_VOID)) return advance();
    return typeSpecifier();
}

// Function for typeSpecifier production rule, returns terminals for supported types
Token Parser::typeSpecifier() {
    if (check(TYPE_INT)) return advance();
    if (check(TYPE_CHAR)) return advance();
    if (check(TYPE_FLOAT)) return advance();

    throw ParseError("Expected type.", peek());
    return advance();
}

//Function for constant production rule, returns terminals for constants
Token Parser::constant() {
    //check if  next token is an int constant
    if (check(INTEGER)) {
        return advance();
    }

    throw ParseError("Expected constant.", peek());

    return advance();
}

//  Expression Algorithms

//Entry for expressions
Expression *Parser::expressionList() {
    try {
        //supposed to build a list of expressions, but decided not to
        Expression *exp = expression();
        return exp;

    } catch(ParseError &se) {
        this->err = true;
        se.printException();

        std::cout << peek().getLexeme() << std::endl;
        synchronize();
    }

    return nullptr;
}

//handles assignment expressions
Expression *Parser::expression() {
    //check for id
    if (check(IDENTIFIER)) {
        //check for assignment ops
        if (checkNext(EQUAL) || checkNext(PLUS_EQUAL) || checkNext(MINUS_EQUAL) || checkNext(STAR_EQUAL) || checkNext(SLASH_EQUAL)) {
            //consume token and operator
            Token id = advance();

            Token op = advance();

            Expression *right = expressionList();

            return new Assignment(id, op, right);
        }
    }

    return simpleExpression();
}

//handles OR expressions
Expression *Parser::simpleExpression() {
    Expression *left = andExpr();
//check for ||
    if (check(BAR_BAR)) {
        //get operator
        Token op = advance();

        Expression * right = simpleExpression();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles and expressions
Expression *Parser::andExpr() {
    Expression *left = bitOrExpr();
//check for &&
    if (check(AND_AND)) {
        //get operaor
        Token op = advance();

        Expression *right = andExpr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles bit or expressions
Expression *Parser::bitOrExpr() {
    Expression *left = bitXorExpr();
//check for |
    if (check(BAR)) {
        //get operaor
        Token op = advance();
        
        Expression *right = bitOrExpr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles xor expressions
Expression *Parser::bitXorExpr() {
    Expression *left = bitAndExpr();
//check for ^
    if (check(XOR)) {
        //get operaor
        Token op = advance();
        
        Expression *right = bitXorExpr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles bit and expresions
Expression *Parser::bitAndExpr() {
    Expression *left = relop1Expr();
//check for &
    if (check(AND)) {
        //get operaor
        Token op = advance();
        
        Expression *right = bitAndExpr();

        return new BinaryExp(left, op, right);
    }

    return left;
}
//handles comparison an *=
Expression *Parser::relop1Expr() {
    Expression *left = relop2Expr();
//check for == and *=
    if (check(EQUAL_EQUAL) || check(BANG_EQUAL)) {
        //get operaor
        Token op = advance();
        
        Expression *right = relop1Expr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles basic boolean operations
Expression *Parser::relop2Expr() {
    Expression *left = bitShiftExpr();
//check for < <= > >=
    if (check(LESS) || check(GREATER) || check(LESS_EQUAL) || check(GREATER_EQUAL)) {
        //get operaor
        Token op = advance();
        
        Expression *right = relop2Expr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles the un implemented bitshifts
Expression *Parser::bitShiftExpr() {
    return sumExpr();
}

//handles arithmatic ops
Expression *Parser::sumExpr() {
    Expression *left = term();
//check for + and -
    if (check(PLUS) || check(MINUS)) {
        //get operaor
        Token op = advance();
        
        Expression *right = sumExpr();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles multiplication ops
Expression *Parser::term() {
    Expression *left = preUnaryExpr();
//check for * / %
    if (check(STAR) || check(SLASH) || check(MOD)) {
        //get operaor
        Token op = advance();
        
        Expression *right = term();

        return new BinaryExp(left, op, right);
    }

    return left;
}

//handles prefix unary expressions
Expression *Parser::preUnaryExpr() {
    //check for++ and --
    if (check(PLUS_PLUS) || check(MINUS_MINUS)) {
        Token op = advance();

        if (check(IDENTIFIER)) {
            Token id = advance();
            Token fixedOp;
            PrimaryExp *value = new PrimaryExp(Token(INTEGER, std::to_string(1), id.getLine(), 1));
            
            if (op.getType() == PLUS_PLUS) {
                fixedOp = Token(PLUS_EQUAL, "+=", id.getLine());
            } else {
                fixedOp = Token(MINUS_EQUAL, "-=", id.getLine());
            }

            return new Assignment(id, fixedOp, value);
        } else {
            throw ParseError("Expected identifier.", peek());
        }
    } 
    //check for + - ! ~ unary ops
    if (check(PLUS) || check(MINUS) || check(BANG) || check(TILDA)) {
        Token op = advance();

        Expression *exp = preUnaryExpr();

        return new UnaryExp(op, exp);
    }

    // return postUnaryExpr();
    return factor();
}

// to be maybe implemented later
// Expression *Parser::postUnaryExpr() {
//     if (check(IDENTIFIER)) {
//         Token id = advance();

//         if (check(PLUS_PLUS) || check(MINUS_MINUS)) {
//             Token op = advance();

//             return new PostUnaryExp(op, new PrimaryExp(id));
//         } else {
//             //throw exception
//         }
//     }

//     return factor();
// }

//handles identifiers in expressions
Expression *Parser::factor() {
    //check for id
    if (check(IDENTIFIER)) {
        //check if the id belongs to a funcall
        if (checkNext(LEFT_PAREN)) {
            return call();
        }

        Token id = advance();

        return new PrimaryExp(id);
    }

    return immutable();
}

//handles group expressions
Expression *Parser::immutable() {
    //check for(
    if (check(LEFT_PAREN)) {
        advance();

        Expression *exp = expressionList();
        //check for )
        if (check(RIGHT_PAREN)) {
            advance();
        } else {
            throw ParseError("Expected ).", peek());
        }

        return new GroupExp(exp);
    }

    return new PrimaryExp(constant());
}

//handles function calls
Expression *Parser::call() {
    Token id = advance();
    std::list<Expression *> arg;

    //check for (
    if (check(LEFT_PAREN)) {
        advance();

        arg = args();
//check for )
        if (check(RIGHT_PAREN)) {
            advance();
        } else {
            throw ParseError("Expected ).", peek());
        }
    } else {
        throw ParseError("Expected (.", peek());
    }

    return new FunCall(id, arg);
}

//handles funcion call arguments
std::list<Expression *> Parser::args() {
    std::list<Expression *> arg;

    while(!check(RIGHT_PAREN)) {
        arg.push_back(expressionList());
        
        if (!check(RIGHT_PAREN)) {
            if (check(COMMA)) {
                advance();
            } else {
                throw ParseError("Expected comma.", peek());
            }
        }
    }

    return arg;
}

// Memory deallocs AST - Christian
void freeAST(std::list<Statement *> &ast) {
    for (std::list<Statement *>::iterator i = ast.begin(); i != ast.end(); ++i)
        delete (*i);  
}
