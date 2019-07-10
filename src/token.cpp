/* File: scanner.cpp 
 * Authors: Elias
 * Description: Implementation of a token, including its lexeme and associated value
 */

#include "token.h"
#include "exceptions.h"

// Constructors for the Token class
// Takes the token Type, a string representation of the lexeme, the line, and
// if needed, an internal representation of the type
// It is overloaded to account for the different types of literal values
// that we can have
// -------------------------------------------------------------------------
Token::Token(TokenType type, std::string lexeme, int line) {
    this->lexeme = lexeme;
    this->type = type;
    this->line = line;
}

Token::Token(TokenType type, std::string lexeme, int line, int value) {
    this->lexeme = lexeme;
    this->type = type;
    this->line = line;
    this->value.ival = value;
}

Token::Token(TokenType type, std::string lexeme, int line, char value) {
    this->lexeme = lexeme;
    this->type = type;
    this->line = line;
    this->value.cval = value;
}

Token::Token(TokenType type, std::string lexeme, int line, float value) {
    this->lexeme = lexeme;
    this->type = type;
    this->line = line;
    this->value.fval = value;
}
// -------------------------------------------------------------------------

// Copy constructor
Token::Token(const Token &tok) {
    type = tok.type;
    lexeme = tok.lexeme;
    line = tok.line;
    value = tok.value;
}

// Return the lexeme stored in the token
std::string Token::getLexeme() {
    return this->lexeme;
}

int Token::getLine() {
    return this->line;
}

// Return the token type stored in the lexeme
TokenType Token::getType() {
    return this->type;
}

// Return the stored literal value.
// Utilizes a template since a cval is not a definite type
template <class T>
T Token::getLiteral() {
    return *(T *)&value;
}

// Overload the '<<' operator
// Let's you put a token object in a std::ostream expresssion
// e.g. std::cout << tok << std::endl;
std::ostream& operator << (std::ostream &out, Token &token) {
    return out << std::string("<Token ") <<
           stringOfTokenType(token.getType()) << std::string(" ") <<
           token.getLexeme() << std::string(">");
}

// Return a string representation of a token type
std::string stringOfTokenType(TokenType type) {
    switch (type) {
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case STAR: return "STAR";
        case SLASH: return "SLASH";
        case LEFT_PAREN: return "LEFT_PAREN";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_BRACK: return "LEFT_BRACK";
        case RIGHT_BRACK: return "RIGHT_BRACK";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case LEFT_BRACE: return "LEFT_BRACE";
        case LESS: return "LESS";
        case GREATER: return "GREATER";
        case EQUAL: return "EQUAL";
        case BANG: return "BANG";
        case TILDA: return "TILDA";
        case SEMICOLON: return "SEMICOLON";
        case LESS_EQUAL: return "LESS_EQUAL";
        case GREATER_EQUAL: return "GREATER_EQUAL";
        case EQUAL_EQUAL: return "EQUAL_EQUAL";
        case BANG_EQUAL: return "BANG_EQUAL";
        case AND: return "AND";
        case AND_AND: return "AND_AND";
        case BAR: return "BAR";
        case BAR_BAR: return "BAR_BAR";
        case MOD: return "MOD";
        case XOR: return "XOR";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case SWITCH: return "SWITCH";
        case CASE: return "CASE";
        case DEFAULT: return "DEFAULT";
        case GOTO: return "GOTO";
        case BREAK: return "BREAK";
        case FOR: return "FOR";
        case WHILE: return "WHILE";
        case CONTINUE: return "CONTINUE";
        case RETURN: return "RETURN";
        case DO: return "DO";
        case IDENTIFIER: return "IDENTIFIER";
        case STRING: return "STRING";
        case INTEGER: return "INTEGER";
        case FLOAT: return "FLOAT";
        case TYPE_INT: return "TYPE_INT";
        case TYPE_CHAR: return "TYPE_CHAR";
        case TYPE_FLOAT: return "TYPE_FLOAT";
        case TYPE_STRING: return "TYPE_STRING";
        case TYPE_VOID: return "TYPE_VOID";
        case EOFILE: return "EOF";  
        
        //Addtional Two Character Tokens
        case PLUS_PLUS: return "PLUS_PLUS";
        case MINUS_MINUS: return "MINUS_MINUS";
        case PLUS_EQUAL: return "PLUS_EQUAL";
        case MINUS_EQUAL: return "MINUS_EQUAL";
        case SLASH_EQUAL: return "SLASH_EQUAL";
        case MOD_EQUAL: return "MOD_EQUAL";
        case STAR_EQUAL: return "STAR_EQUAL";
        case LBITSHIFT: return "LBITSHIFT";
        case RBITSHIFT: return "RBITSHIFT";
        default: return "unknown";
    }
}
