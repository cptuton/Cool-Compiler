/* File: scanner.h
 * Authors: Elias
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "token.h"

// The abstract class CompilerException should not be instantiated
//Instead, please extend it with a more descriptive class for your needs

// Abstract Exception class
class CompilerException {
protected:
    std::string err;
    int line;

    std::string getLineInfo();
public:
    virtual void printException();
};

// Error to be thrown when the Scanner class encounters an error
class ScanError : public CompilerException {
public:
    ScanError(std::string err, int line);
    void printException();
};

// Error to be thrown when we try to free an un-handled token type
class UnboundToken : public CompilerException {
public:
    UnboundToken(std::string err, int line);
    void printException();
};

// Error to be thrown when the parser encounters an error
class ParseError : public CompilerException {
public:
    ParseError(std::string err, Token tok); // The token contains the line number
    void printException();
};

// Error to be thrown when there is and error associated with the IR
class IR_Error : public CompilerException {
public:
    IR_Error(std::string err);
    void printException();
};

// Error to be thrown when there is a syntax error (currently symbol table)
class SyntaxError : public CompilerException {
public:
    SyntaxError(std::string err, Token tok); // The token contains the line number
    void printException();
};

// Error to be thrown when there is a code generation error
class CodeGenError : public CompilerException {
public:
    CodeGenError(std::string err);
    void printException();
};

#endif
