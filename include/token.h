/* File: token.h
 * Authors: Elias, Christian
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

#include "c_type.h"

enum TokenType {
    // One and two char tokens
    PLUS, MINUS, STAR, SLASH, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACK, RIGHT_BRACK,
    LEFT_BRACE, RIGHT_BRACE, LESS, GREATER, EQUAL, BANG, SEMICOLON, LESS_EQUAL,
    GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL, COMMA, MOD, AND, BAR, XOR, BAR_BAR,
    AND_AND, COLON, TILDA,
    
    // Additional two char tokens
    PLUS_PLUS, MINUS_MINUS, PLUS_EQUAL, MINUS_EQUAL,
    SLASH_EQUAL, STAR_EQUAL, MOD_EQUAL, LBITSHIFT, RBITSHIFT,

    // Keywords
    IF, ELSE, SWITCH, BREAK, FOR, WHILE, RETURN, DO, GOTO, DEFAULT, CONTINUE, CASE, 

    // Variable
    IDENTIFIER, STRING, INTEGER, FLOAT,

    // Types
    TYPE_INT, TYPE_CHAR, TYPE_FLOAT, TYPE_STRING, TYPE_VOID,

    // End of file
    EOFILE
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    int line;
    union ctype value;
public:
    Token() = default;
    Token(TokenType type, std::string lexeme, int line);
    Token(TokenType type, std::string lexeme, int line, int value);
    Token(TokenType type, std::string lexeme, int line, char value);
    Token(TokenType type, std::string lexeme, int line, float value);
    // Copy constructor
    Token(const Token &tok);
    // Return the lexeme
    std::string getLexeme();
    // Return the token type
    TokenType getType();
    // Return the line 
    int getLine();
    // Return the literal value stored
    template <class T>
    T getLiteral();
    // Similar to Java's toString method 
    friend std::ostream& operator << (std::ostream &out, Token &token);
};

std::string stringOfTokenType(TokenType type);

#endif
