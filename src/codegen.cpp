/** File: codegen.cpp
 *  Authors: Christian, Michael
 *  Description: Structure and helper functions for the transition from IR to x86
 */

#include "codegen.h"

// Constructor for CPU
CPU::CPU() {

    //init free register stack
    freeRegStack.push_back("rax");
    freeRegStack.push_back("rbx");
    freeRegStack.push_back("rcx");
    freeRegStack.push_back("rdx");
    freeRegStack.push_back("r8 ");
    freeRegStack.push_back("r9");
    freeRegStack.push_back("r10");
    freeRegStack.push_back("r11");
    freeRegStack.push_back("r12");
    freeRegStack.push_back("r13");
    freeRegStack.push_back("r14");
    freeRegStack.push_back("r15");

    //init register free status
    usage["rax"] = true;
    usage["rbx"] = true;
    usage["rcx"] = true;
    usage["rdx"] = true;
    usage["r8"] = true;
    usage["r9"] = true;
    usage["r10"] = true;
    usage["r11"] = true;
    usage["r12"] = true;
    usage["r13"] = true;
    usage["r14"] = true;
    usage["r15"] = true;
}

void CPU::BuildRange(std::vector<BasicBlock> &bblist) {
    int line = 1;
    for(auto &block : bblist) {
        block.iter([&line, this](Instruction *inst) {
            switch (inst->getOp()) {
            case CONST_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    this->regRange[reg].first = line;
                }

                Container op1 = ((LetInstr *)inst)->getOp1();

                if (op1.isRegister()) {
                    std::string op = "%r" + std::to_string(op1.getIntValue());
                    this->regRange[op].second = line;
                }
                break;
            }
            case UNARY_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    this->regRange[reg].first = line;
                }

                Container op1 = ((LetInstr *)inst)->getOp1();

                if (op1.isRegister()) {
                    std::string op = "%r" + std::to_string(op1.getIntValue());
                    this->regRange[op].second = line;
                }
                break;
            }
            case BINARY_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    this->regRange[reg].first = line;
                }

                Container op1 = ((LetInstr *)inst)->getOp1();

                if (op1.isRegister()) {
                    std::string op = "%r" + std::to_string(op1.getIntValue());
                    this->regRange[op].second = line;
                }

                Container op2 = ((LetInstr *)inst)->getOp2();

                if (op2.isRegister()) {
                    std::string op = "%r" + std::to_string(op2.getIntValue());
                    this->regRange[op].second = line;
                }

                break;
            }
            case BRANCH: {
                Container cont = ((BranchInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    this->regRange[reg].second = line;
                }
                break;
            }
            case RET: {
                Container cont = ((ReturnInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    this->regRange[reg].second = line;
                }
                break;
            }
            default:
                break;
            }

            line++;
        });
    }
}

std::vector<BasicBlock> CPU::assignRegs(std::vector<BasicBlock> &bblist) {
    std::vector<BasicBlock> blocks;
    std::list<Instruction *> ir;
    int line = 0;

    for(auto &block : bblist) {
        block.iter([&line, &ir, this](Instruction *inst) {
            switch (inst->getOp()) {
            case CONST_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                std::string vReg;
                std::string pReg;
                
                Container op1 = ((LetInstr *)inst)->getOp1();
                std::string opVr;
                std::string opPr;

                if (op1.isRegister()) {
                    opVr = "%r" + std::to_string(op1.getIntValue());
                    opPr = this->Ensure(opVr);

                    if (this->regRange[opVr].second >= line) {
                        this->Free(opPr);
                    }

                    ContainerValue val;
                    val.physReg = new std::string(opPr);

                    op1 = Container(PREG, val);
                }

                if (cont.isRegister()) {
                    vReg = "%r" + std::to_string(cont.getIntValue());
                    pReg = this->Allocate(vReg);

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);
                }

                LetInstr *i = new LetInstr(cont, CONST_LET, NULL);
                i->setExpression(op1);
                ir.push_back(i);
                break;
            }
            case UNARY_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                std::string vReg;
                std::string pReg;
                
                Container op1 = ((LetInstr *)inst)->getOp1();
                std::string opVr;
                std::string opPr;

                if (op1.isRegister()) {
                    opVr = "%r" + std::to_string(op1.getIntValue());
                    opPr = this->Ensure(opVr);

                    if (this->regRange[opVr].second >= line) {
                        this->Free(opPr);
                    }

                    ContainerValue val;
                    val.physReg = new std::string(opPr);

                    op1 = Container(PREG, val);
                }

                if (cont.isRegister()) {
                    vReg = "%r" + std::to_string(cont.getIntValue());
                    pReg = this->Allocate(vReg);

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);
                }

                LetInstr *i = new LetInstr(cont, UNARY_LET, NULL);
                i->setExpression(op1, ((LetInstr *)inst)->getOperation());
                ir.push_back(i);

                break;
            }
            case BINARY_LET: {
                Container cont = ((LetInstr *)inst)->getContainer();
                std::string vReg;
                std::string pReg;
                
                Container op1 = ((LetInstr *)inst)->getOp1();
                std::string opVr;
                std::string opPr;

                if (op1.isRegister()) {
                    opVr = "%r" + std::to_string(op1.getIntValue());
                    opPr = this->Ensure(opVr);

                    ContainerValue val;
                    val.physReg = new std::string(opPr);

                    op1 = Container(PREG, val);
                }

                Container op2 = ((LetInstr *)inst)->getOp2();
                std::string op2Vr;
                std::string op2Pr;

                if (op2.isRegister()) {
                    op2Vr = "%r" + std::to_string(op2.getIntValue());
                    op2Pr = this->Ensure(op2Vr);

                    ContainerValue val;
                    val.physReg = new std::string(op2Pr);

                    op2 = Container(PREG, val);
                }

                if (op1.isPhysRegister()) {
                    if (this->regRange[opVr].second >= line) {
                        this->Free(opPr);
                    }
                }
                if (op2.isPhysRegister()) {
                    if (this->regRange[op2Vr].second >= line) {
                        this->Free(op2Pr);
                    }
                }

                if (cont.isRegister()) {
                    vReg = "%r" + std::to_string(cont.getIntValue());
                    pReg = this->Allocate(vReg);

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);
                }

                LetInstr *i = new LetInstr(cont, BINARY_LET, NULL);
                i->setExpression(op1, op2, ((LetInstr *)inst)->getOperation());
                ir.push_back(i);

                break;
            }
            case BRANCH: {
                Container cont = ((BranchInstr *)inst)->getContainer();
                std::string vReg;
                std::string pReg;

                if (cont.isRegister()) {
                    vReg = "%r" + std::to_string(cont.getIntValue());
                    pReg = this->Ensure(vReg);

                    if (this->regRange[vReg].second >= line) {
                        this->Free(pReg);
                    }

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);
                }
                std::string one = ((BranchInstr *)inst)->getLabelOne();
                std::string *two = ((BranchInstr *)inst)->getLabelTwo();
                BranchInstr *i = new BranchInstr(cont, one, two, NULL);
                // BranchInstr *i = new BranchInstr(cont, ((BranchInstr *)inst)->getLabelOne(), ((BranchInstr *)inst)->getLabelTwo(), NULL);
                ir.push_back(i);
                break;
            }
            case RET: {
                Container cont = ((ReturnInstr *)inst)->getContainer();
                std::string vReg;
                std::string pReg;

                if (cont.isRegister()) {
                    vReg = "%r" + std::to_string(cont.getIntValue());
                    pReg = this->Ensure(vReg);

                    if (this->regRange[vReg].second >= line) {
                        this->Free(pReg);
                    }

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);
                }
                ReturnInstr *i = new ReturnInstr(cont, NULL);
                ir.push_back(i);
                break;
            }

            case CALL: {
                std::list<Container> *args = ((CallInstr *)inst)->getArgs();
                std::list<Container> *params = new std::list<Container>();
                std::list<std::string> vRegs;

                for(auto arg : *args) {
                    std::string vReg;
                    std::string pReg;

                    if (arg.isRegister()) {
                        vReg = "%r" + std::to_string(arg.getIntValue());

                        pReg = this->Ensure(vReg);

                        ContainerValue val;
                        val.physReg = new std::string(pReg);

                        arg = Container(PREG, val);
                    }

                    params->push_back(arg);
                }

                for(auto vReg : vRegs) {
                    if (this->regRange[vReg].second >= line) {
                        this->Free(vrToPr[vReg]);
                    }
                }

                Container cont = ((CallInstr *)inst)->getContainer();
                if (cont.isRegister()) {
                    std::string vReg;
                    std::string pReg;
                    vReg = "%r" + std::to_string(cont.getIntValue());

                    pReg = this->Ensure(vReg);

                    ContainerValue val;
                    val.physReg = new std::string(pReg);

                    cont = Container(PREG, val);

                    if (this->regRange[vReg].second >= line) {
                        this->Free(pReg);
                    }
                }

                CallInstr *call = new CallInstr(((CallInstr *)inst)->getName(), params, NULL);
                call->setExpression(cont);

                ir.push_back(call);
                break;
            }
            default:
                ir.push_back(inst);
                break;
            }

            line++;
        });
    }

    blocks = bblist_of_instruction_list(ir);
    removeEmptyBlocks(bblist);

    return blocks;
}

void CPU::printRange() {
    for (std::map<std::string, std::pair<int, int>>::iterator p = regRange.begin(); p != regRange.end(); ++p) {
            
        std::cout << p->first << " "<< p->second.first << ", "<< p->second.second <<std::endl;
    }
}

std::string CPU::Ensure(std::string vr) {
    auto iter = vrToPr.find(vr);

    //if vr is already mapped
    if (iter != vrToPr.end()) {
        return vrToPr[vr];
    }

    std::string result = Allocate(vr);
    vrToPr[vr] = result;
    //emit code to move vr to result?

    return result;
}

std::string CPU::Allocate(std::string vr) {
    std::string reg;
    if (freeRegStack.size() > 0) {
        reg = freeRegStack.back();
        freeRegStack.pop_back();
    } else {
        //revoke a register
        //store revoked reg contents
    }
    //map vr to physical reg
    vrToPr[vr] = reg;
    //mark as unavailable
    usage[reg] = false;

    return reg;
}

void CPU::Free(std::string reg) {
    std::string vr = "";
    
    for (auto p = vrToPr.begin(); p != vrToPr.end(); ++p) {
        if(p->second == reg) {
            vr = p->first;
            break;
        }
    }

    vrToPr.erase(vr);
    usage.erase(vr);
    freeRegStack.push_back(reg);
}

// Program
// -----------------------------------------------------------------

// helpers, not methods of Program

// convert a container into asm representation
std::string Program::toAssembly(Container c) {
    switch (c.type) {
    case STR: //variable
    {
        // not sure what to do here
        // should move stack offset into register
        if (findGlobal(c.getStringValue()))
            return c.getStringValue() + "(%rip)";
        int off = allocVar(c.getStringValue());
        return "-" + std::to_string(off) + "(%rbp)";
    }
    case IMM:
        return "$" + std::to_string(c.getIntImmediate());
    case REG:
        return "%r" + std::to_string(c.getIntValue());
    case PREG:
        return "%" + c.getReg();
    case NIL:
        return "";
    }

    return "";
}

// create an arbitrary branch label
std::string createJumpLabel() {
    static int i = 0;
    i++;

    return ".JL" + std::to_string(i);
}

std::string tab() {
    return "    ";
}

// Convert a let expression into assembly
// description of how the macros work in codegen.h
std::string Program::toAssembly(LetInstr *li) {

#define BINOP(operation) \
    do { \
        std::string builder; \
        builder = tab() + "movq " + toAssembly(li->getOp1()) + ", %rax\n"; \
        builder = tab() + "movq " + toAssembly(li->getOp2()) + ", %rbx\n"; \
        builder += tab() + operation + " %rax, %rbx\n"; \
        builder += tab() + "movq %rbx, " + toAssembly(li->getContainer()) + "\n"; \
        return builder; \
    }while (false); \

#define UNOP(operation) \
    do { \
        std::string builder; \
        builder = tab() + "movq " + toAssembly(li->getOp1()) + ", %rax\n"; \
        builder += tab() + operation + " %rax\n"; \
        builder += tab() + "movq %rax, " + toAssembly(li->getContainer()) + "\n"; \
        return builder; \
    }while (false); \

#define LOGIC_OP(jumpType) \
    do { \
        std::string builder; \
        std::string dest = toAssembly(li->getContainer()); \
        std::string op1 = toAssembly(li->getOp1()); \
        std::string op2 = toAssembly(li->getOp2()); \
        std::string lbl1 = createJumpLabel(); \
        std::string lbl2 = createJumpLabel(); \
        builder = tab() + "cmp  $0, " + op1 + "\n"; \
        builder += tab() + jumpType + " " + lbl1 + "\n"; \
        builder += tab() + "cmp $0, " + op2 + "\n"; \
        builder += tab() + jumpType + " " + lbl1 + "\n"; \
        builder += tab() + "movq $1, " + dest + "\n"; \
        builder += tab() + "jmp " + lbl2 + "\n"; \
        builder += lbl1 + ":\n"; \
        builder += tab() + "movq $0, " + dest + "\n"; \
        builder += lbl2 + ":\n"; \
        return builder; \
    }while(false); \

#define COMPAR(jumpType) \
    do { \
        std::string builder; \
        std::string dest = toAssembly(li->getContainer()); \
        std::string op1 = toAssembly(li->getOp1()); \
        std::string op2 = toAssembly(li->getOp2()); \
        std::string lbl1 = createJumpLabel(); \
        std::string lbl2 = createJumpLabel(); \
        builder = tab() + "movq " + op1 + ", " + dest + "\n"; \
        builder += tab() + "subq " + op2 + ", " + dest + "\n"; \
        builder += tab() + jumpType + " " + lbl1 + "\n"; \
        builder += tab() + "movq $0, " + dest + "\n"; \
        builder += tab() + "jmp " + lbl2 + "\n"; \
        builder += lbl1 + ":\n"; \
        builder += tab() + "movq $1, " + dest + "\n"; \
        builder += lbl2 + ":\n"; \
        return builder; \
    }while (false); \

    switch (li->getOperation()) {
    case IrOp::ADD: BINOP("addq");
    case IrOp::SUB:
        if (toAssembly(li->getContainer()) != std::string("")) {
            BINOP("subq");
        }
        else {
            UNOP("negq");
        }
    case IrOp::MUL: BINOP("mulq");
    case IrOp::DIV:
    {
        std::string builder;
        std::string dest = toAssembly(li->getContainer());
        std::string op1 = toAssembly(li->getOp1());
        std::string op2 = toAssembly(li->getOp2());
        builder = tab() + "xorq %rdx, %rdx\n";
        builder += tab() + "movq " + op1 + ", %rax\n";
        builder += tab() + "movq " + op2 + ", %rbx\n";
        builder += tab() + "divq %rbx\n";
        builder += tab() + "movq %rax, " + dest + "\n";
        return builder;
    }
    case IrOp::LESS: COMPAR("jl");
    case IrOp::GREATER: COMPAR("jg");
    case IrOp::LESS_EQ: COMPAR("jle");
    case IrOp::GREATER_EQ: COMPAR("jge");
    case IrOp::NOT_EQ: COMPAR("jne")
    case IrOp::MOD:
    {
        std::string builder;
        std::string dest = toAssembly(li->getContainer());
        std::string op1 = toAssembly(li->getOp1());
        std::string op2 = toAssembly(li->getOp2());
        builder = tab() + "xorq %rdx, %rdx\n";
        builder += tab() + "movq " + op1 + ", %rax\n";
        builder += tab() + "movq " + op2 + ", %rbx\n";
        builder += tab() + "divq %rbx\n";
        builder += tab() + "movq %rdx, " + dest + "\n";
        return builder;
    }
    case IrOp::AND: BINOP("andq");
    case IrOp::OR: BINOP("orq");
    case IrOp::AND_AND: LOGIC_OP("je");
    case IrOp::OR_OR: LOGIC_OP("jne");
    case IrOp::XOR: BINOP("xorq");
    case IrOp::EQUAL_EQUAL: COMPAR("je");
    case IrOp::BFLIP: UNOP("notq");
    case IrOp::NOT:
    {
        std::string builder;
        std::string dest = toAssembly(li->getContainer());
        std::string op1 = toAssembly(li->getOp1());
        builder = tab() + "xorq %rax, %rax\n";
        builder += tab() + "movq " + op1 + ", %rbx\n";
        builder += tab() + "test %rbx, %rbx\n";
        builder += tab() + "sete %al\n";
        builder += tab() + "movq %rax, " + dest + "\n";
        return builder;
    }
    default:
        BINOP("UNHANDLED");
    }

    return "";
#undef BINOP
#undef COMPAR
#undef LOGIC_OP
#undef UNOP
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Program::Program(std::string fileName) {
     stack_level =  0;
    std::map<std::string, int> temp2;

    stack.push_back(1);
    offsets.push_back(temp2);

    file = ".file \"" + fileName + "\"\n";

    data = ".section .data\n";

    bss = ".section .bss\n";

    text = ".section .text\n";

    inFunction = false;

    numArgs = 0;
}

std::string Program::toString() {
    return file + "\n" + data + "\n" + bss + "\n" + text + "\n"; // I like having an extra newline at the end
}

void Program::declareFunction(std::string name) {
    text += tab() + ".globl " + name + "\n";
    text += tab() + ".type " + name + ", @function\n";
    text += name + ":\n";
    text += tab() + "pushq %rbp\n";
    text += tab() + "movq %rsp, %rbp\n";
}

bool Program::findGlobal(std::string name) {
    for (std::string i : globals) {
        if (i == name)
            return true;
    }
    return false;
}

// Insert instructions from a basic block into the assembly program
void Program::insertBasicBlock(BasicBlock &bb) {

    bb.iter([&](Instruction *i) {
        switch (i->getOp()) {
        case DEF:
        {
            declareFunction(((DefInstr *)i)->getName());
            newFunc();
            inFunction = true; // we are now in a function
            numArgs = 0; // start with zero local variables

            int k = 16;
            if (((DefInstr *)i)->getParams()->size() > 0) {
                text += tab() + "subq $" + std::to_string(((DefInstr *)i)->getParams()->size() * 8) + ", %rsp\n";
                numArgs += ((DefInstr *)i)->getParams()->size();
            }
            for (auto &cont : *((DefInstr *)i)->getParams()) {
                int off = allocVar(cont.getStringValue());
                text += tab() + std::string("movq ") + std::to_string(k) + "(%rbp), %rax\n";
                text += tab() + std::string("movq ") + "%rax, -" + std::to_string(off) + "(%rbp)\n";
                k += 8;
            }
        }
        break;
        case LABEL:
            text += ((LableInstr *)i)->getLabelName() + ":\n";
            break;
        case JUMP:
            text += tab() + "jmp " + ((JumpInstr *)i)->getLabelName() + "\n";
            break;
        case RET:
            // deallocate variables
            if (numArgs > 0)
                text += tab() + "addq $" + std::to_string(numArgs * 8) + ", %rsp\n";
            
            if (toAssembly(((ReturnInstr *)i)->getContainer()) != std::string(""))
                text += tab() + "movq " + toAssembly(((ReturnInstr *)i)->getContainer()) + ", %rax\n";

            text += tab() + "popq %rbp\n";
            text += tab() + "retq\n";
            break;
        // We only allow global variables of the type:
        // x = constant
        // usually our optimizations will let that pass
        case BINARY_LET:
            // we only allow constant global variables
            if (!inFunction) {
                throw CodeGenError(
                    std::string("Non-constant value for global variable '") +
                    ((LetInstr *)i)->getContainer().getStringValue() + "'.");
            }
            text += toAssembly((LetInstr *)i);
            break;
        case CONST_LET:
            if (!inFunction) {
                std::string name = ((LetInstr *)i)->getContainer().getStringValue();
                Container c = ((LetInstr *)i)->getOp1();
                if (!c.isImmediate()) {
                    throw CodeGenError(
                        std::string("Non-constant value for global variable '") +
                        name + "'.");
                }
                data += "#----------------------------\n";
                data += "# Global Var: " + name + "\n";
                data += tab() + ".globl " + name + "\n";
                data += tab() + ".data\n";
                data += tab() + ".type " + name + ", @object\n";
                data += name + ":\n";
                data += tab() + ".quad " + std::to_string(c.getIntValue()) + "\n";
                data += "#----------------------------\n";
                globals.push_back(name);
            } else {
                text += tab() + "movq " + toAssembly(((LetInstr *)i)->getOp1()) + ", " + toAssembly(((LetInstr *)i)->getContainer()) + "\n";
            }
            break;
        case UNARY_LET:
           // we only allow constant global variables
            if (!inFunction) {
                throw CodeGenError(
                    std::string("Non-constant value for global variable '") +
                    ((LetInstr *)i)->getContainer().getStringValue() + "'.");
            } 
            text += toAssembly((LetInstr *)i);
            break;
        case BRANCH:
            text += tab() + "cmp $1, " + toAssembly(((BranchInstr *)i)->getContainer()) + "\n";
            if (((BranchInstr *)i)->getLabelTwo() != NULL)
                text += tab() + "jne " + *(((BranchInstr *)i)->getLabelTwo()) + "\n";
            else
                text += tab() + "jne " + ((BranchInstr *)i)->getLabelOne() + "\n";
            break;
        case END:
            // deallocate variables
            if (numArgs > 0)
                text += tab() + "addq $" + std::to_string(numArgs * 8) + ", %rsp\n";

            // we are no longer in a function
            text += tab() + "popq %rbp\n";
            text += tab() + "retq\n";

            inFunction = false;
            break;
        case DECL:
            allocVar(((DeclInstr *)i)->getContainer().getStringValue());
            text += tab() + "pushq $0\n";
            numArgs++;
            break;
        case CALL:
            for (auto j : *(((CallInstr *)i)->getArgs())) {
                text += tab() + "pushq " + toAssembly(j) + "\n";
            }
            text += tab() + "callq " + ((CallInstr *)i)->getName() + "\n";
            if (((CallInstr *)i)->getArgs()->size() > 0)
                text += tab() + "addq $" + std::to_string(((CallInstr *)i)->getArgs()->size() * 8) + ", %rsp\n";

            // if there is a return value, store it in the destination
            if (toAssembly(((CallInstr *)i)->getContainer()) != std::string("")) {
                text += tab() + "movq %rax, " + toAssembly(((CallInstr *)i)->getContainer()) + "\n";
            }
            break;
        default:
            text += tab() + i->toString() + "\n"; // for testing
        }
    });
}

// Sends 8 bytes to next available slot in memory
//      returns index of used memory
int Program::allocVar(std::string var) {
    if (offsets[stack_level].find(var) != offsets[stack_level].end()) {
        return offsets[stack_level][var];
    }

    int off = offset(stack[stack_level]);
    stack[stack_level]++;
    offsets[stack_level][var] = off;

    return off;
}

void Program::newFunc() {
    std::map<std::string, int> temp2;
    
    stack.push_back(1);
    offsets.push_back(temp2);
    stack_level++;
}

// Returns actual address offset (4 * value)
int Program::offset(int index) {
    return 8 * index;
}

// -----------------------------------------------------------------
