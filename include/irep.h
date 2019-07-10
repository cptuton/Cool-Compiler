/* File: irep.h
 * Authors: Nathan, MichaeL, Elias
 * Description: Header for irep.cpp and defines the structure of the intermediate representation
 */

#ifndef IREP_H
#define IREP_H

#include <list>
#include <functional>
#include <memory>
#include <set>

#include "symbol.h"
#include "token.h"
#include "c_type.h"
#include "exceptions.h"

// Declare operator types 
enum OpType {
    END, DECL, DEF, UNARY_LET, BINARY_LET, CONST_LET, CALL, BRANCH, LABEL, JUMP, RET
};

// Declare the Container Type Either string, immediate, or register
enum ContainerType {
    STR, IMM, PREG, REG, NIL
};

// IR expression operation
enum class IrOp {
    ADD, SUB, MUL, DIV, NOT, ADD_EQ, SUB_EQ, MUL_EQ, DIV_EQ, LESS, GREATER, LESS_EQ, GREATER_EQ,
    NOT_EQ, MOD, MOD_EQ, AND, OR, AND_AND, OR_OR, XOR, BFLIP, PLUS_PLUS,
    MINUS_MINUS, EQUAL_EQUAL
};

// The union type to handle either the string name or register value
union ContainerValue {
    // virtual register number
    int reg;
    //physical register string
    std::string *physReg;
    // variable name
    std::string *varName;
    // the literal value
    union ctype cval;
};

// Contains the type and value of the dual type to distinguish between registers and values
struct Container {
    // the container type
    ContainerType type;
    // the value of the container type
    ContainerValue value;
    //The symbol Type
    SymbolType sym;

    //copy constructor
    Container(const Container &cont);

    // Constructor
    Container(ContainerType type, ContainerValue value);
    Container(ContainerType type, std::string *varName);
    Container(ContainerType type, std::string *varName, SymbolType sym);
    Container(ContainerType type, int reg);
    Container(ContainerType type);
    Container() = default;
    // set literal value
    void setVal(int val);
    void setVal(char val);
    void setVal(float val);
    // Check functions
    bool isRegister();
    bool isPhysRegister();
    bool isVariable();
    bool isImmediate();
    // Getter
    int getIntValue();
    int getIntImmediate();
    std::string getReg();
    std::string getStringValue();
    std::string getSymType();
    //
    std::string toString();
};

// Refactoring the IR for more flexibility
// Allowing each operator defined in the enum to be its own class inherting from instruction
class Instruction {
    protected:
        SymbolTable *currentSymbolTable;
        OpType operation;
    public:
        // toStringing for debugging / outputing
        OpType getOp();
        virtual std::string toString();

        //virtual ~Instruction();

};

// let statements (let container = value binary_op value, 
// where value = immediate or container)
class LetInstr : public Instruction {
    private:
        Container letContainer;
        Container operand1;
        Container operand2;
        IrOp op;
    public:
        //not finished, still needs the expression
        LetInstr(Container input, OpType op, SymbolTable *table);

        void setExpression(Container operand1, Container operand2, IrOp op);
        void setExpression(Container operand1, IrOp op);
        void setExpression(Container operand1);
        Container getContainer();
        Container getOp1();
        Container getOp2();
        IrOp getOperation();
        virtual std::string toString() override;

};

// declarations (decl variable type)
class DeclInstr : public Instruction {
    private:
        Container declContainer;

    public:
        DeclInstr(std::string *name, SymbolType type, SymbolTable *table);
        virtual std::string toString() override;
        Container getContainer();

};

// Function Calls and Definitions need paramlist and arglsit respectively
// function definitions (def @fun_name (paramlist) -> type {})
class DefInstr: public Instruction {
    private:
        std::string funName;
        SymbolType funType;
        std::list<Container> *params;
    public:
        DefInstr(std::string name, SymbolType type, std::list<Container> *params, SymbolTable *table);
        virtual std::string toString() override;
        std::string getName();
        std::list<Container> *getParams();

};

// Function Call
// call @ID
class CallInstr: public Instruction {
    private:
        std::string funName;
        std::list<Container> *args;
        Container regCapture;
        int regCaptureFlag = 0;
        
    public:
        Container getContainer();
        int getFlag();
        std::list<Container> *getArgs();
        std::string getName();
        void setExpression(Container captureReg);
        CallInstr(std::string name, std::list<Container> *args, SymbolTable *table);
        virtual std::string toString() override;

};

// Label
// label ID
class LableInstr: public Instruction {
    private:
        std::string lableName;
    public:
        LableInstr(std::string name, SymbolTable *table);
        std::string getLabelName();
        virtual std::string toString() override;

};
// Need a way to store condition statements
// If EXP LABLE1 LABEL2
class BranchInstr: public Instruction {
    private:
        std::string lableOne;
        std::string *lableTwo = nullptr;
        Container cond;
    public:
        Container getContainer();
        std::string getLabelOne();
        std::string *getLabelTwo();
        BranchInstr(Container cond, std::string one, std::string *two, SymbolTable *table);
        virtual std::string toString() override;

};

// Jump
// jump LABEL
class JumpInstr: public Instruction {
        std::string lable;
    public:
        JumpInstr(std::string dest, SymbolTable *table);
        virtual std::string toString() override;
        std::string getLabelName();
};

// return
// return expr (return nil for voids?)
class ReturnInstr: public Instruction {
    private:
        Container returnValue;
    public:
        ReturnInstr(Container value, SymbolTable *table);
        ReturnInstr(SymbolTable *table);
        Container getContainer();
        virtual std::string toString() override;

};

class FuncEnd: public Instruction {
    public:
        FuncEnd(SymbolTable *table);
        virtual std::string toString() override;

};

/* Basic block
 * separates the basic blocks of functionality in the intermediate representation
 * what denotes a basic block leader (from wikipedia):
 * 1. First instruction
 * 2. The target of a conditional or an unconditional goto/jump instruction
 * 3. The instruction that immediately follows a conditional or an
 *    unconditional goto/jump instruction
 *
 * what denotes the end of a basic block:
 * 1. unconditional and conditional branches, both direct and indirect
 * 2. returns to a calling procedure
 * 3. the return instruction itself
 *
 * what denotes the start of a block:
 * 1. procedure and function entry points
 * 2. targets of jumps or branches
 * 3. "fall-through" instructions following some conditional branches
 */
class BasicBlock {
private:
    std::list<Instruction *> instructions;
public:
    BasicBlock() = default;

    // Basic Block will take over responsibility of
    // freeing memory
    void insertInstruction(Instruction *instruction);

    // iterate over list of instructions and apply fun to each
    // element
    void iter(std::function<void (Instruction *)> fun);

    // is the list empty?
    bool isEmpty();
};

// helper functions

// convert an IR op into a string
std::string string_of_irop(IrOp op);

// convert a token version of an operation into an IR version of an operation
IrOp irop_of_token(Token t);

// convert a list of instructions to a list of basic blocks
std::vector<BasicBlock> bblist_of_instruction_list(std::list<Instruction *>instructions);

// remove empty blocks from a list of basic blocks
void removeEmptyBlocks(std::vector<BasicBlock> &bblist);

#endif
