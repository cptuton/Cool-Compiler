/* File: irep.cpp
 * Authors: Michael, NAtHaN
 * Description: implementation for intermediate representation data structures
 */

#include "irep.h"

// Container Structure
// -----------------------------------------------------------------
//constructors
Container::Container(ContainerType type, ContainerValue value) {
    this->type = type;
    this->value = value;
}

Container::Container(ContainerType type, std::string *varName) {
    this->type = type;
    this->value.varName = varName;
}

Container::Container(ContainerType type, std::string *varName, SymbolType sym) {
    this->type = type;
    this->value.varName = varName;
    this->sym = sym;
}

Container::Container(ContainerType type, int reg) {
    this->type = type;
    this->value.reg = reg;
}

Container::Container(ContainerType type) {
    this->type = type;
}

// copy constructor
Container::Container(const Container &cont) {
    type = cont.type;
    value = cont.value;
    sym = cont.sym;
}

//set union int value
void Container::setVal(int val) {
    value.cval.ival = val;
}

//set union char value
void Container::setVal(char val) {
    value.cval.cval = val;
}

//set union float value
void Container::setVal(float val) {
    value.cval.fval = val;
} 

//boolean check if container is for a register
bool Container::isRegister() {
    return type == REG;
}

bool Container::isPhysRegister() {
    return type == PREG;
}
//boolean check if container is for a variable
bool Container::isVariable() {
    return type == STR;
}

//boolean check if container is for a immediate
bool Container::isImmediate() {
    return type == IMM;
}

/**
 *Used to get string of symbol type.
 */
std::string Container::getSymType() {
    switch(this->sym) {
        case SYM_INT:
            return "int";
        case SYM_CHAR:
            return "char";
        case SYM_FLOAT:
            return "float";
        default:
            return "Undifined";
    }
    return "err";
}

//gets int union value
int Container::getIntValue() {
    return value.reg;
}

int Container::getIntImmediate() {
    return value.cval.ival;
}


//gets the string union value
std::string Container::getReg() {
    return *value.physReg;
}

std::string Container::getStringValue() {
    return *value.varName;
}
//converts the container to a string
std::string Container::toString() {
    std::string full = "";

    switch (this->type) {
        case STR: 
            full = std::string(*(value.varName)); 
            break;
        case IMM: 
            full = std::to_string(value.cval.ival); 
            break;
        case REG: 
            full = "%r" + std::to_string(value.reg); 
            break;
        case PREG:
            full = std::string(*(value.physReg));
            break;
        case NIL: 
            full = "NIL"; 
            break;
    }

    return full;
}

// Instruction
// -----------------------------------------------------------------
std::string Instruction::toString() { return "";}
OpType Instruction::getOp() { return this->operation;}


// -----------------------------------------------------------------

// -----------------------------------------------------------------

// Let Instruction
// -----------------------------------------------------------------
//constructor
LetInstr::LetInstr(Container input, OpType op, SymbolTable *table):
    letContainer(input) {
    this->operation = op;
    this->currentSymbolTable = table;
}

//sets variables for binary expression
void LetInstr::setExpression(Container operand1, Container operand2, IrOp op) {
    this->operand1 = operand1;
    this->operand2 = operand2;
    this->op = op;
}

//sets variables for unary expression
void LetInstr::setExpression(Container operand1, IrOp op) {
    this->operand1 = operand1;
    this->op = op;
}

//sets variables for constant expression
void LetInstr::setExpression(Container operand1) {
    this->operand1 = operand1;
}

//convers the let instr to a string and return it
std::string LetInstr::toString() {
    std::string contStr = "let " + this->letContainer.toString();

    switch(this->operation) {
        case UNARY_LET:
            contStr += " = " + string_of_irop(op) + " " + operand1.toString();
            break;
        case BINARY_LET:
            contStr += " = " + operand1.toString() + " ";
            contStr += string_of_irop(op) + " " + operand2.toString();
            break;
        case CONST_LET:
            contStr += " = " + operand1.toString();
            break;
        default:
            break;
    }

    return contStr;
}

Container LetInstr::getContainer() {
    return this->letContainer;
}

Container LetInstr::getOp1() {
    return this->operand1;
}

Container LetInstr::getOp2() {
    return this->operand2;
}

IrOp LetInstr::getOperation() {
    return this->op;
}

// -----------------------------------------------------------------

// Declaration Instruction
// -----------------------------------------------------------------
//constructor
DeclInstr::DeclInstr(std::string *name, SymbolType type, SymbolTable *table):
    declContainer(Container(STR, name, type)) {
    this->operation = DECL;
    this->currentSymbolTable = table;
}
//converts decl instr to string 
std::string DeclInstr::toString() {
    std::string ret = "decl " + declContainer.toString() + " ";

    return ret + declContainer.getSymType();
}

Container DeclInstr::getContainer() {
    return this->declContainer;
}

// -----------------------------------------------------------------

// Definition Instruction
// -----------------------------------------------------------------
//constructor
DefInstr::DefInstr(std::string name, SymbolType type, std::list<Container> *params, SymbolTable *table) {
    this->funName = name;
    this->funType = type;
    this->operation = DEF;
    this->currentSymbolTable = table;
    this->params = params;
}

//converts def instr to string 
std::string DefInstr::toString() {
    std::string ret = "def @" + this->funName + "(";

    //dont hate me, i had to write the loop this way to fix the formatting
    for (std::list<Container>::iterator i = this->params->begin(); i != this->params->end(); ++i) {        
        if (i == std::prev(this->params->end())) {
            ret += i->getSymType() + " " + i->toString();
        } else{
            ret += i->getSymType() + " " + i->toString() + ", ";
        }
    }

    switch(this->funType) {
        case SYM_INT:
            ret +=  ") -> int";
            break;
        case SYM_CHAR:
            ret +=  ") -> char";
            break;
        case SYM_FLOAT:
            ret += ") -> float";
            break;
        case SYM_VOID:
            ret += ") -> void";
            break;
        default:
            return "Err";
    }
    return ret;
}

std::string DefInstr::getName() {
    return funName;
}

std::list<Container> *DefInstr::getParams() {
    return params;
}

// -----------------------------------------------------------------

// Call Instruction
// -----------------------------------------------------------------
//constructor

CallInstr::CallInstr(std::string name, std::list<Container> *args, SymbolTable *table) {
    this->funName = name;
    this->operation = CALL;
    this->currentSymbolTable = table;
    this->args = args;
    this->regCaptureFlag = 0;
}
Container CallInstr::getContainer() {
    return this->regCapture;
}
int CallInstr::getFlag() {
    return this->regCaptureFlag;
}
std::list<Container> *CallInstr::getArgs() {
    return this->args;
}

std::string CallInstr::getName() {
    return this->funName;
}

void CallInstr::setExpression(Container captureReg) {
    this->regCapture = captureReg;
    this->regCaptureFlag = 1;
}

//converts call instr to string 
std::string CallInstr::toString() {
    std::string ret = "call@ " + this->funName + "(";

    //dont hate me, i had to write the loop this way to fix the formatting
    for (std::list<Container>::iterator i = this->args->begin(); i != this->args->end(); ++i) {        
        if (i == std::prev(this->args->end())) {
            ret += i->toString();
        } else{
            ret += i->toString() + ", ";
        }
    }
    
    // Check if the flag was set
    // std::cout << "Reg Capture Flag " <<this->regCaptureFlag << " RegCaptureRegister: " << this->regCapture.toString() << std::endl;
    if (this->regCaptureFlag == 1) {
        ret += ") -> ";
        ret += this->regCapture.toString();
        return ret;
    } else {
        return ret += ")";
    }
}
// -----------------------------------------------------------------

// Label Instruction
// -----------------------------------------------------------------
//constructor
LableInstr::LableInstr(std::string name, SymbolTable *table) {
    this->lableName = name;
    this->operation = LABEL;
    this->currentSymbolTable = table;
}

//converts lable instr to string 
std::string LableInstr::toString() {
    return "label " + this->lableName;
}

// getter for the label name
std::string LableInstr::getLabelName() {
    return this->lableName;
}

// -----------------------------------------------------------------

// Branch Instruction
// -----------------------------------------------------------------
BranchInstr::BranchInstr(Container conditional, std::string one, std::string *two, SymbolTable *table) {
    this->cond = conditional;
    this->lableOne = one;
    this->lableTwo = two;
    this->operation = BRANCH;
    this->currentSymbolTable = table;
}

Container BranchInstr::getContainer() {
    return this->cond;
}
std::string BranchInstr::getLabelOne() {
    return this->lableOne;
}

std::string *BranchInstr::getLabelTwo() {
    return this->lableTwo;
}

//converts branch instr to string
std::string BranchInstr::toString() {
    std::string ret = "if " + cond.toString();
    ret += " " + lableOne + " ";
    
    if (lableTwo) {
        ret += *lableTwo;
    }

    return ret;
}

// -----------------------------------------------------------------

// Jump Instruction
// -----------------------------------------------------------------
JumpInstr::JumpInstr(std::string dest, SymbolTable *table) {
    this->lable = dest;
    this->operation = JUMP;
    this->currentSymbolTable = table;
}
std::string JumpInstr::toString() {
    return "jump " + lable;
}

std::string JumpInstr::getLabelName() {
    return lable;
}

// -----------------------------------------------------------------

// Return Instruction
// -----------------------------------------------------------------
ReturnInstr::ReturnInstr(Container value, SymbolTable *table):
    returnValue(value) {
    this->operation = RET;
    this->currentSymbolTable = table;
}

ReturnInstr::ReturnInstr(SymbolTable *table):
    returnValue(Container(NIL)) {
    this->operation = RET;
    this->currentSymbolTable = table;
}

Container ReturnInstr::getContainer() {
    return this->returnValue;
}

std::string ReturnInstr::toString() {
    return "return " + returnValue.toString();
}

// -----------------------------------------------------------------

// FuncEnd
// -----------------------------------------------------------------

FuncEnd::FuncEnd(SymbolTable *table) {
    this->operation = END;
    this->currentSymbolTable = table;
}

std::string FuncEnd::toString() {
    return "funcend";
}
// -----------------------------------------------------------------

// Basic Block
// -----------------------------------------------------------------

void BasicBlock::insertInstruction(Instruction *instruction) {
    instructions.push_back(instruction);
}

void BasicBlock::iter(std::function<void (Instruction *)> fun) {
    for (auto *i : instructions) {
        fun(i);
    }
}

bool BasicBlock::isEmpty() {
    return instructions.size() == 0;
}

// -----------------------------------------------------------------

// Helpers

std::string string_of_irop(IrOp op) {
    switch (op) {
        case IrOp::ADD: return "+";
        case IrOp::SUB: return "-";
        case IrOp::MUL: return "*";
        case IrOp::DIV: return "/";
        case IrOp::NOT: return "!";
        case IrOp::ADD_EQ: return "+=";
        case IrOp::SUB_EQ: return "-=";
        case IrOp::MUL_EQ: return "*=";
        case IrOp::DIV_EQ: return "/=";
        case IrOp::LESS: return "<";
        case IrOp::GREATER: return ">";
        case IrOp::LESS_EQ: return "<=";
        case IrOp::GREATER_EQ: return ">=";
        case IrOp::NOT_EQ: return "!=";
        case IrOp::MOD: return "%";
        case IrOp::MOD_EQ: return "%=";
        case IrOp::AND: return "&";
        case IrOp::OR: return "|";
        case IrOp::AND_AND: return "&&";
        case IrOp::OR_OR: return "||";
        case IrOp::XOR: return "^";
        case IrOp::BFLIP: return "~";
        case IrOp::PLUS_PLUS: return "++";
        case IrOp::MINUS_MINUS: return "--";
        case IrOp::EQUAL_EQUAL: return "==";
        default:
            throw IR_Error("Unrecognized operator.");
    }
}

IrOp irop_of_token(Token t) {
    switch (t.getType()) {
        case PLUS:          return IrOp::ADD;
        case PLUS_EQUAL:    return IrOp::ADD_EQ;
        case MINUS:         return IrOp::SUB;
        case MINUS_EQUAL:   return IrOp::SUB_EQ;
        case STAR:          return IrOp::MUL;
        case STAR_EQUAL:    return IrOp::MUL_EQ;
        case SLASH:         return IrOp::DIV;
        case SLASH_EQUAL:   return IrOp::DIV_EQ;
        case LESS:          return IrOp::LESS;
        case GREATER:       return IrOp::GREATER;
        case LESS_EQUAL:    return IrOp::LESS_EQ;
        case GREATER_EQUAL: return IrOp::GREATER_EQ;
        case BANG:          return IrOp::NOT;
        case EQUAL_EQUAL:   return IrOp::EQUAL_EQUAL;
        case BANG_EQUAL:    return IrOp::NOT_EQ;
        case MOD:           return IrOp::MOD;
        case MOD_EQUAL:     return IrOp::MOD_EQ;
        case AND:           return IrOp::AND;
        case BAR:           return IrOp::OR;
        case XOR:           return IrOp::XOR;
        case BAR_BAR:       return IrOp::OR_OR;
        case AND_AND:       return IrOp::AND_AND;
        case TILDA:         return IrOp::BFLIP;
        case PLUS_PLUS:     return IrOp::PLUS_PLUS;
        case MINUS_MINUS:   return IrOp::MINUS_MINUS;
        default:
            throw IR_Error("Unhandled token conversion.");
    }
}

// create a list of basic blocks from a list of instructions
std::vector<BasicBlock> bblist_of_instruction_list(std::list<Instruction *>instructions) {
    // initialize basic block list
    std::vector<BasicBlock> bblist;

    // helper functions
    // --------------------------------------------

    // start a new basic block
    std::function<void (void)> pushBB = [&](void) {
        bblist.push_back(BasicBlock());
    };

    // add an instruction to the current basic block
    std::function<void (Instruction *)> addInstruction = [&](Instruction *i) {
        bblist.back().insertInstruction(i);
    };
    // --------------------------------------------

    // add the initial basic block to the list
    bblist.push_back(BasicBlock());

    // the first instruction is always a leader
    bblist[0].insertInstruction(*instructions.begin());

    for (Instruction *i : instructions) {

        // already added the first instruction
        if (i == *instructions.begin()) continue;

        switch (i->getOp()) {
            // variable declarations are insignificant
            case DECL:
                addInstruction(i);
                break;
            case DEF: // beginning of function is start of basic block
                pushBB();
                addInstruction(i);
                break;
            // lets are insignificant
            case UNARY_LET:
            case BINARY_LET:
            case CONST_LET:
                addInstruction(i);
                break;
            // function calls are insignificant
            case CALL:
                addInstruction(i);
                break;
            // branches end a basic block, so the next instruction should be
            // a leader
            case BRANCH:
                addInstruction(i);
                pushBB();
                break;
            // labels start a basic block
            case LABEL:
                pushBB();
                addInstruction(i);
                break;
            // jumps end a basic block, next instruction should be leader
            case JUMP:
                addInstruction(i);
                pushBB();
                break;
            // returns end a basic block
            case RET:
                addInstruction(i);
                pushBB();
                break;
            // FuncEnd ends a basic block
            case END:
                addInstruction(i);
                pushBB();
                break;
            default:
                throw IR_Error("Unhandled IR instruction.");
        }
    }

    return bblist;
}

// remove empty basic blocks
void removeEmptyBlocks(std::vector<BasicBlock> &bblist) {
    for (auto it = bblist.begin(); it != bblist.end(); it++) {
        if (it->isEmpty() && (it + 1) == bblist.end()) {bblist.erase(it); return;}
        if (it->isEmpty()) it = bblist.erase(it);
    }
}
