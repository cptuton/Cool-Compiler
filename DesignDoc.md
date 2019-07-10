__Group Members__
  * Michael Anderson
  * Elias Flores
  * Nathan Nguyen
  * Christian Tuton
  

#### Usage
To design and create a simple C style compiler using C++ aimed at compiling a subset of the C language. 
Respectively implementing the front, middle and back end of the compiler as the class advances.

Development of the compiler is intended to include the following baseline features.
With additional effort being required to implement extra features.
Lists may grow or shrink depending on the scope of class.

__Baseline Features__
  * Functions
  * Variables
  * While Loops
  * Integers (System Length)
  * Arithmetic
  * Assignment
  * Boolean Expressions
  * If/Else Statments
  * Urnary operands
  * Returns
  * Breaks
  
__Additional Features__
  * Floats 
  * For Loops
  * Additional Opperands (+=, +-, ++, --, etc.)
  * Binary Operations
  * Switch Statements

A list of C style key words to be recognized in accordance to the simple C language. 
Will grow or shrink as necessary.

__Key Words__
  * int
  * bool
  * if
  * else
  * return
  * break
  * while
  * void
  * do
  * break
  * switch
  * do
  * goto
  * default
  * continue
  * case
  * string
  * float
  

#### Out Of Bounds
The following is a list of advanced features that scale past the intended scope of the class and as such will not be included in this project.
The list may grow or shrink depndeding on the current meta of the course.

__Not to be Implemented__
  * Pointers
  * Arrays
  * Compiler Preproccessing
  * Structures / Enumerators
  * Library Calls
  * Memory Allocation
  * Type Casting and Promotion
  * Type Specifiers
  * Strings


#### Current Objective
Projet milestone markers indicative of current progress of the project with relative start and completion dates with respect to fluid due dates.

__Parser: 1/30/2019 - 2/8/2019__
* Be able to plug in tokenized input into the parser program which will then follow the constructed grammar for this project and output a relative parse tree.
* Switched From LR(1) parsing algorithm to recursive descent parsing as of 2/6/2019


__Scanner: 1/23/2019 - 1/30/2019__
* Be able to read through a source string and tokenize each part.
* Added additional characters into the scanner as of 2/6/2019

__File Size: 1/16/2019 - 1/23/2019__
* Be able to read through a file and return its total character count.

#### Functions
A brief overview of implemented functions with their intended use and return

__filesize.cpp__
* Takes in filename after reading the user input flag -s filename.
* Scans file and returns the total character count of the input file.

__scanner.cpp__
* Building upon the filesize program, this program scans a source string and creates a list of recognized tokens.

__parser.cpp__
* Following the rules of our defined grammar (see below) this program parses a token list that the placeholder.c file scanner function tokenized to validate if this is a proper C program.

#### Style Guide - Taken from README.md
For the most part we will following the kernel coding standards with some tweaks/emphasis:

  * We will be using 4-space tabs instead of 8
  * Function braces are on the same line as the funtion declaration
  * There should be no whitespace between a function call and its argument list
  * Conditional statements should have a space between the right most paranthesis and its brace
    (example: if (cond) {)
  * Else statements should be on the same line as the if's ending brace
  * Use include guards in header files instead of '#pragma once' since there __may__ be issues that arise from the #pragma directive
  * Include guards should be in all caps and be the same name as the file's name, suffixed with \_H
    (example for a file named scanner.h: SCANNER_H)
  * Function names and variables should be in camel case
  * If a function takes no arguments leaving the paranthesis blank is fine
  * Class names should be capitalized
  * Add debugging code with '#ifdefine DEBUG ... #endif' which can be controlled with the build system
  * Do not import a namespace with 'using' into the global scope
  * Create a comment header at the start of every source and header file, describing the name, authors, a description, and any bugs or todos if necessary
    (example:
```
    /* File: source.c
     * Authors: Member1, Member2
     * Description: Does a thing
     * Bugs: Everything
     * Todo: Something
     */
```
  * Create a comment block at the top of every function, describing Authors, a description, variable definitions if necessary, bugs and todos if necessary
  * Please make an attempt to comment your code in logically defined blocks, or specific lines if it is convulated.
  * // Comment like this. (It also looks nicer if the first capitalized)
  * Class prototypes should be declared in a header file and its implementation should be in a source file
)

#### Grammar
The following is the current subset of the C language grammar written in Backus-Naur Form (BNF) form that our group designed from the C- grammar.
This grammar may be subject to change if it is found there are signifcant flaws or ambiguity in its flow. 
```
<program> ::= <declList>

<declList> ::= <declList> <declaration> 
                | <declaration>

<declaration> ::= <varDecl> 
                | <function> 
                | <funcProto>

--------------------------------------------------------------------------------

<varDecl> ::= <typeSpecifier> <varDecList>;

<typeSpecifier> ::= “int” 
                    | “char” 
                    | “float”

<varDecList> ::= <varDecList> “,” <varDecInit> 
                | <varDecInit>

<varDecInit> ::= ID 
                | ID “=” <simpleExpression>

--------------------------------------------------------------------------------

<function> ::= <funcDecl> <block>

<funcProto> ::= <funcDecl> “;”

<funcDecl> ::= <returnType> ID “(” <params> “)”

<returnType> ::= <typeSpecifier> 
                | “void”

<params> ::= <paramList> 
                | EPSILON 
                | “void”

<paramList> ::= <paramList>, <typeSpecifier> ID 
                | <typeSpecifier> “ID”

--------------------------------------------------------------------------------

<statementList> ::= <statementList> <statement> 
                    | <statement>

<statement> ::= <expressionStmt> 
                | <block> 
                | <selectionStmt> 
                | <iterationStmt> 
                | <returnStmt> 
                | <breakStmt> 
                | <function> 
                | <gotoStmt> 
                | <label> 
                | <varDecl>

<returnStmt> ::= “return” <expressionStmt>

<block> ::= “{” <statementList> “}”

<breakStmt> ::= “break” “;”

<selectionStmt> ::= <ifStmt> 
                    | <switchStmt>

<ifStmt> ::=   “if” “(” <expressionList> “)” <statement>
                | “if” “(” <expressionList> “)” <statement> “else” <statement>

<switchStmt> ::= “switch” “(” <expressionList> “)” “{” <caseList> “}”

<caseList> ::= EPSILON 
                | <caseList> <caseStmt> 
                | <caseStmt>

<caseStmt> ::= “case” <expression> “:” <statementList> 
                | “default” “:” <statementList> 
                | <breakStmt>

<gotoStmt> ::= “goto” ID “;”

<label> ::= ID “:” 

--------------------------------------------------------------------------------

<expressionStmt> ::= <expressionList> “;” 
                        | “;”

<iterationStmt> ::= <whileStmt> 
                    | <forStmt>

<whileStmt> ::= “while” “(“ <expressionList> “)” <statement>

<forStmt> ::= “for” “(“ <expressionList>; <expressionList>; <expressionList> “)” <statement>

<expressionList> ::= <expressionList> “,” <expression> 
                    | <expression>

<expression> ::= ID = <expressionList> 
                | ID += <expressionList> 
                | ID −= <expressionList>
                | ID ∗= <expressionList> 
                | ID /= <expressionList> 
                | <simpleExpression>

--------------------------------------------------------------------------------

<simpleExpression> ::= <simpleExpression> “||” <andExpr> 
                        | <andExpr>

<andExpr> ::= <andExpr> “&&” <bitOrExpr> 
                | <bitOrExpr>

<bitOrExpr> ::= <bitOrExpr> “|” <bitXorExpr> 
                | <bitXorExpr>

<bitXorExpr> ::= <bitXorExpr> “^” <bitAndExpr> 
                | <bitAndExpr>

<bitAndExpr> ::= <bitAndExpr> “&” <relop1Expr> 
                | <relop1Expr>

<relop1Expr> ::= <relop1Expr> <relop1> <relop2Expr> 
                | <relop2Expr>

<relop1> ::= “==” 
            | “!=”

<relop2Expr> ::= <relop2Expr> <relop2> <bitShiftExpr> 
                | <bitShiftExpr>

<relop2> ::= “<” 
            | “<=” 
            | “>” 
            | “>=”

<bitShiftExpr> ::= <bitShiftExpr> <bitShiftOp> <sumExpr> 
                    | <sumExpr>

<sumExpr> ::= <sumExpr> <sumop> <term> 
            | <term>

<sumop> ::= “+” 
            | “-”

<term> ::= <term> <mulop> <unaryExpr> 
            | <unaryExpr>

<mulop> ::= "∗" 
            | "/" 
            | "%"

<preUnaryExpr> ::= “++” ID
                	| “--” ID
                	| <preUnaryExpr> <unaryop>
                	| <postUnaryExpr>
	
<unaryop> ::= “+” 
            | “-” 
            | “!” 
            | “~”

<postUnaryExpr> ::=  ID “++”
                	| ID “--”
                	| <factor>

<factor> ::= <immutable> 
            | ID

--------------------------------------------------------------------------------

<immutable> ::= ( <expressionList> ) 
                | <call> 
                | <constant> 

<call> ::= ID ( <args> ) 

<args> ::= <expressionList> 
            | EPSILON

<constant> ::= NUMCONST 
                | FLOATCONST
```
