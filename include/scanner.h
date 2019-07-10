/* File: scanner.h
 * Authors: Elias, Christian
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <list>
#include <map>
#include <vector>


#include "token.h"
#include "exceptions.h"

typedef std::pair<std::string, TokenType> keyTok;

class Scanner {
    private:
        std::map<std::string, TokenType> keywords;
        std::string source;
        std::vector<Token> tokens;
        int line;
        int start;
        int current;
        bool err;

        // Adds the next recognized token to the token list unless an error occurs
        void scanToken();
        // Returns true if the scanner has reached the end of the source string
        bool isAtEnd();
        // Advances to the next character in the source string
        char advance();
        // Adds a token with the type 'type' to the token list
        void addToken(TokenType type);
        void addToken(TokenType type, int value);
        void addToken(TokenType type, char value);
        void addToken(TokenType type, float value);
        // Advances to the next character in the source string only if the current char
        // matches the given one. Returns true if we advance, false otherwise
        bool matchChar(char c);
        // Return the current character without advancing
        char peek();
        // Scans the source string from the current position until it hits a '"'
        // and creates a string token
        void captureString();
        // Same as captureString, but it captures characters and throws errors if not
        // formatted correctly, then creates a character token
        void captureCharacter();
        // Handles numeric input (ints, floats, ect.)
        void handleNumber();
        // Returns true if c is a valid identifier character: (_ | [a-z A-Z] | [0-9])
        bool isIdChar(char c);
        // Returns true if c is an alpha character: [a-z A-Z]
        bool isAlpha(char c);
        // Returns true if c is a digi: [0-9]
        bool isDigit(char c);
        // Scans the source string from the current position until it matches '*' '/'
        void multiLineComment();
    public:
        Scanner(std::string source);
        // start scanning the source string and return a list of tokens found in it
        std::vector<Token> lex();
        // returns true if the scanner encountered an error, false otherwise
        bool hadError();
};

#endif
