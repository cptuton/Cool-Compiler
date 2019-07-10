/* File: scanner.cpp
 * Authors: Elias
 * Description: Define a set of exceptions to be used when an error is encountered
 */

#include <iostream>
#include "exceptions.h"

// Compiler Exception

// Print out the exception
// This method should be overriden
void CompilerException::printException() {
    std::cout << "Exception Occurred." << std::endl;
}

// Return the line information where the exception was thrown
// This should not be overriden
std::string CompilerException::getLineInfo() {
    return "[line " + std::to_string(line) + "]";
}

// ----------------------------------------------------
// Scan Error Exception

// ScanError constrcutor that takes in a description of the error and the line
// it occurred on.
// Used when the scanner encounters an error
ScanError::ScanError(std::string err, int line) {
    this->err = err;
    this->line = line;
}

// Override the printException method
void ScanError::printException() {
    std::cout  << getLineInfo() << " Scan Error: " << err << std::endl;
}

// ----------------------------------------------------
// Ubound Token Exception

// UnboundToken constructor that takes in a description of the error and the line
// it occurred on.
// Used when a token is encountered that has a value associated with it that we
// don't know how to free.
UnboundToken::UnboundToken(std::string err, int line) {
    this->err = err;
    this->line = line;
}

// Override the printException method
void UnboundToken::printException() {
    std::cout  << getLineInfo() << " Unbound Token: " << err << std::endl;
}

// ----------------------------------------------------
// Parse Error Exception

// ParseError constrcutor that takes in a description of the error and the
// offending token.
// Used when the Parser encounters an error
ParseError::ParseError(std::string err, Token tok) {
    this->err = err;
    this->line = tok.getLine();
}

// Override the printException method
void ParseError::printException() {
    std::cout  << getLineInfo() << " ParseError: " << err << std::endl;
}

// ----------------------------------------------------
// Intermediate Representation Exception

// IR_Error constrcutor that takes in a description of the error
// Used when the IR encounters an error
IR_Error::IR_Error(std::string err) {
    this->err = err;
}

// Override the printException method
void IR_Error::printException() {
    std::cout << "IR Error: " << err << std::endl;
}

// ----------------------------------------------------
// Syntax Error Exception (symbol table)

// SyntaxError constrcutor that takes in a description of the error and the
// offending token.
// Used when the symbol table construction in parse tree encounters an error
SyntaxError::SyntaxError(std::string err, Token tok) {
    this->err = err;
    this->line = tok.getLine();
}

// Override the printException method
void SyntaxError::printException() {
    std::cout  << getLineInfo() << " SyntaxError: " << err << std::endl;
}
// ----------------------------------------------------

// Code Generation Exception

// Used when the code generation step encounters an
// error
CodeGenError::CodeGenError(std::string err) {
    this->err = err;
}

// Override the printException method
void CodeGenError::printException() {
    std::cout << " Code Gen Error: " << err << std::endl;
}
// ----------------------------------------------------
