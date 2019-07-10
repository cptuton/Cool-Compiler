/* File: scanner.cpp 
 * Authors: Elias, Nathan, Christian
 * Description: Scans a source string and creates a list of recognized tokens
 * Todo:
   1. Implement being able to read a source in chunks
   2. - can be in front of a number so a rule should be added for this case
   3. std::map searches in O(log(n)) time and inserts in O(log(n)) time
      it'd be nice if we could get the search to O(c)
   4. Recognize some more tokens
 */

#include "scanner.h"

#ifdef DEBUG
#include <iostream>
#endif

// Constructor for the Scanner
// Takes in the source code as a string
Scanner::Scanner(std::string source) {
    this->source = source;
    line = 1;
    start = 0;
    current = 0;
    err = false;

    // keytok defined in scanner.h
    // std::pair<std::string, TokenType>
    keywords.insert( keyTok("if", IF) );
    keywords.insert( keyTok("else", ELSE) );
    keywords.insert( keyTok("switch", SWITCH) );
    keywords.insert( keyTok("case", CASE) );
    keywords.insert( keyTok("break", BREAK) );
    keywords.insert( keyTok("default", DEFAULT) );
    keywords.insert( keyTok("continue", CONTINUE) );
    keywords.insert( keyTok("goto", GOTO) );
    keywords.insert( keyTok("for", FOR) );
    keywords.insert( keyTok("while", WHILE) );
    keywords.insert( keyTok("do", DO) );
    keywords.insert( keyTok("return", RETURN) );
    keywords.insert( keyTok("int", TYPE_INT) );
    keywords.insert( keyTok("char", TYPE_CHAR) );
    keywords.insert( keyTok("float", TYPE_FLOAT) );
    keywords.insert( keyTok("string", TYPE_STRING) );
    keywords.insert( keyTok("void", TYPE_VOID) );
}

// Create a list of tokens found in the source string
std::vector<Token> Scanner::lex() {
#ifdef DEBUG
    std::cout << "Scanning Source String" << std::endl;
#endif

    // scan tokens until we reach the end or an error occurs
    while (!isAtEnd()) {
        start = current;

        try {
            scanToken();
        }
        catch (ScanError &se) {
            // If we encounter an error, set the error flag and keep
            // scanning to catch more possible errors
            se.printException();
            err = true;
        }
    }

    tokens.push_back(Token(EOFILE, "", line));

    return tokens;
}

// Scan a token and add it to the token list
// We scan from the starting character until the current character
// no longer fits the current classifier
// For example, if we're recognizing an integer, when we hit a non digit, we convert
// what we have into an integer token:
// [1][2][+][3]
//  |     |
// start current
void Scanner::scanToken() {
    char c = advance();

    switch (c) {
        // Simple tokens
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '[': addToken(LEFT_BRACK); break;
        case ']': addToken(RIGHT_BRACK); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ';': addToken(SEMICOLON); break;
        case ',': addToken(COMMA); break;
        case '^': addToken(XOR); break;
        case ':': addToken(COLON); break;
        case '~': addToken(TILDA); break;   // 2/18/19 Christian Tilda support
        
        // longest match always wins
        // <= becomes [<=] instead of [<] [=]
        // orthogonal becomes [orthogonal] instead of [or] [thogonal]
        case '!': addToken(matchChar('=') ? BANG_EQUAL : BANG); break;
        case '=': addToken(matchChar('=') ? EQUAL_EQUAL : EQUAL); break;
        case '|': addToken(matchChar('|') ? BAR_BAR : BAR); break;
        case '&': addToken(matchChar('&') ? AND_AND : AND); break;

        //Code for Additional Double tokens (++, +=, --, -=, *=, %=, /=) 
        //Removed + , -, % and * from simple tokens 
        //Nathan Nguyen 1/28/2019
        case '*': addToken(matchChar('=') ? STAR_EQUAL : STAR); break;
        case '%': addToken(matchChar('=') ? MOD_EQUAL : MOD); break;

        case '+': 

            if (matchChar('+')) {
                addToken(PLUS_PLUS);
            } else if (matchChar('=')) {
                addToken(PLUS_EQUAL);
            }
            else {
                addToken(PLUS); 
            }
            break;

        case '-': 

            if (matchChar('-')) {
                addToken(MINUS_MINUS);
            } else if (matchChar('=')) {
                addToken(MINUS_EQUAL);
            } else {
                addToken(MINUS); 
            }
            break;
        // Code for bit shifting, Christian 2/18/19
        case '<':
            if (matchChar('<'))
                addToken(LBITSHIFT);
            else if (matchChar('='))
                addToken(LESS_EQUAL);
            else
                addToken(LESS);
            break;
        case '>':
            if (matchChar('>'))
                addToken(RBITSHIFT);
            else if (matchChar('='))
                addToken(GREATER_EQUAL);
            else
                addToken(GREATER);
            break;
        // Match whitespace
        case ' ':
        case '\t':
        case '\r':
            break;

        // Match newline
        case '\n':
            line++;
            break;

        // Match string
        case '"':
            captureString();
            break;
        
        // Match character
        case '\'':
            captureCharacter();
            break;

        // Match a comment
        case '/':
            if (matchChar('/')) {
                // Ignore line comment
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                // Ignore multi line comment
            } else if (matchChar('*')) {
                multiLineComment();
            } else if (matchChar('=')) {
                // /= token
                addToken(SLASH_EQUAL);
            } else {
                // It's just a slash
                addToken(SLASH);
            }
            break;
        // Floating point
        case '.':
            while (isDigit(peek())) advance();
            addToken(FLOAT, std::stof(source.substr(start, current)));
            break;

        default:
            // Integer -?[0-9]+
            // scan until we hit a non digit character
            if (isDigit(c)) {

                handleNumber();
            // Identifier (_ | [alpha])(_ | [alphanumeric])*
            // The first character is an alpha/underline character
            // everything else can be alphanumeric
            } else if (isAlpha(c)) {
                while (isIdChar(peek())) advance();

                // Is the identifier a reserved word?
                std::string value = source.substr(start, current - start);
                auto it = keywords.find(value);
                
                if (it != keywords.end()) {
                    // The second item in the pair is the token type
                    addToken(it->second);
                } else {
                    // If it's not a token, then it's an identifier
                    addToken(IDENTIFIER);
                }
            } else {
                throw ScanError("Unexpected character '" + std::string(1, c) + "'.", line);
            }
    }
}

// Add a token to the token list
// Overloaded to allow tokens with literal values
// --------------------------------------------------------
void Scanner::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
   
    tokens.push_back(Token(type, text, line));
}

void Scanner::addToken(TokenType type, int value) {
    std::string text = source.substr(start, current - start);

    tokens.push_back(Token(type, text, line, value));
}

void Scanner::addToken(TokenType type, char value) {
    std::string text = source.substr(start, current - start);

    tokens.push_back(Token(type, text, line, value));
}

void Scanner::addToken(TokenType type, float value) {
    std::string text = source.substr(start, current - start);

    tokens.push_back(Token(type, text, line, value));
}
// --------------------------------------------------------

// Are we at the end of the string?
bool Scanner::isAtEnd() {
    return current >= (int)source.length();
}

// Advance to the next character in the source string
// Return the character we were just at
char Scanner::advance() {
    current++;
    return source.at(current - 1);
}

// Acts like a conditional advance
// Advance only if the given character matches the current character
bool Scanner::matchChar(char c) {
    if (isAtEnd() || source[current] != c)
        return false;

    current++;
    return true;
}

// Return the current character
char Scanner::peek() {
    if (isAtEnd()) return '\0';

    return source[current];
}

// Scan a string until we reach the ending "
void Scanner::captureString() {
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd())
        throw ScanError("Unterminated String.", line);

    // consume and ignore the last "
    advance();

    std::string value = source.substr(start + 1, current - 1);
    // addToken(STRING, value);
    throw ScanError("Strings are not supported.", line);
}

// Scan a character sequence until we reach the ending "
void Scanner::captureCharacter() {
    while (!isAtEnd() && peek() != '\'') {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd())
        throw ScanError("Unterminated Character", line);

    // consume and ignore the last "
    advance();

    std::string value = source.substr(start + 1, current - 1);
    // addToken(CHAR, value);
    throw ScanError("Character constants are not supported", line);
}


// Handles number interpretation
void Scanner::handleNumber() {
    
    while (isDigit(peek())) advance();
    
    // Floating point
    if (peek() == '.') {
        advance();
        while(isDigit(peek())) advance();
        float f = std::stof(source.substr(start, current));
        addToken(FLOAT, f);

    // Integer
    } else {
        int x = std::stoi(source.substr(start, current));
        addToken(INTEGER, x);
    }
}

// If we match '/*' then ignore everything until
// we get to the end of the comment
void Scanner::multiLineComment() {
    // advance until we encounter a * followed by a /
    while (!(matchChar('*') && matchChar('/'))) {
        // If we get to a newline, don't forget to increment the line
        if (matchChar('\n')) {
            line++;
        } else if (isAtEnd()) {
            throw ScanError("Unterminated Comment.", line);
        } else {
            advance();
        }
    }
}

// Is the character a valid identifier character?
bool Scanner::isIdChar(char c) {
    return isAlpha(c) || isDigit(c);
}

// Is the character an alpha character or an underline?
bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

// Is the character a digit?
bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::hadError() {
    return err;
}

