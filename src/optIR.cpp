#include "optIR.h"

Optimizer::Optimizer(std::list<Instruction *> IR) {
    this->IR = IR;
}

//function that handles constant propigation
void Optimizer::constProp() {
    std::map<std::string, int> constants;
    std::list<Instruction *> IR2;

    for(auto instr: this->IR) {
        switch(instr->getOp()) {
            case CONST_LET: {
                Container reg = ((LetInstr *)instr)->getContainer();
                Container constant = ((LetInstr *)instr)->getOp1();

                if (reg.isRegister()) {
                    if (constant.isImmediate()) {
                        std::string temp = "%r" + std::to_string(reg.getIntValue());
                        constants[temp] = constant.getIntImmediate();
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    for(auto instr: this->IR) {
        switch(instr->getOp()) {
            case CONST_LET: {
                Container cont = ((LetInstr *)instr)->getOp1();
                std::string reg;
                //if container is an immediate do next itteration
                if (cont.isImmediate()) {
                    break;
                }

                //if register replace and add to new
                if (cont.isRegister()) {
                    reg = "%r" + std::to_string(cont.getIntValue());

                    if (constants.find(reg) != constants.end()) {

                        Container rhs = Container(IMM, constants[reg]);
                        ((LetInstr *)instr)->setExpression(rhs);                    
                    }
                }

                //add instr to new list
                IR2.push_back(instr);
                break;
            }
            case UNARY_LET:{
                Container cont = ((LetInstr *)instr)->getOp1();
                if (cont.isRegister()) {
                    std::string reg = "%r" + std::to_string(cont.getIntValue());
                    
                    //replace
                    if (constants.find(reg) != constants.end()) {
                        Container rhs = Container(IMM, constants[reg]);
                        ((LetInstr *)instr)->setExpression(rhs, ((LetInstr *)instr)->getOperation());
                    }
                }

                //add instr to new list
                IR2.push_back(instr);
                break;
            }
            case BINARY_LET:{
                Container cont = ((LetInstr *)instr)->getOp1();
                Container cont2 = ((LetInstr *)instr)->getOp2();
                std::string reg;
                std::string reg2;

                if (cont.isRegister()) {
                    reg = "%r" + std::to_string(cont.getIntValue());
                
                    if (constants.find(reg) != constants.end()) {
                        //replace cont
                        cont = Container(IMM, constants[reg]);
                    }

                }

                if (cont2.isRegister()) {
                    reg2 = "%r" + std::to_string(cont2.getIntValue());
                    
                    if (constants.find(reg2) != constants.end()) {
                        //replace cont2
                        cont2 = Container(IMM, constants[reg2]);                        
                    }

                }

                //add instr to new list
                ((LetInstr *)instr)->setExpression(cont, cont2, ((LetInstr *)instr)->getOperation());
                IR2.push_back(instr);
                break;
            }
            case CALL:{
                std::list<Container> *args = ((CallInstr *)instr)->getArgs();
                std::list<Container> *new_args = new std::list<Container>();

                for(auto arg : *args) {
                    std::string reg;

                    if (arg.isRegister()) {
                        reg = "%r" + std::to_string(arg.getIntValue());

                        if (constants.find(reg) != constants.end()) {
                            Container temp = Container(IMM, constants[reg]);

                            new_args->push_back(temp);

                        } else {
                            new_args->push_back(arg);
                        }

                    }
                }
                Container cont = ((CallInstr *)instr)->getContainer();

                CallInstr *call = new CallInstr(((CallInstr *)instr)->getName(), new_args, NULL);
                call->setExpression(cont);
                IR2.push_back(call);

                break;
            }
            case BRANCH:{
                Container cond =  ((BranchInstr *)instr)->getContainer();
                std::string reg;

                if (cond.isRegister()) {
                    reg = "%r" + std::to_string(cond.getIntValue());
                    //replace
                    if (constants.find(reg) != constants.end()) {
                        cond = Container(IMM, constants[reg]);
                        std::string one = ((BranchInstr *)instr)->getLabelOne();
                        std::string *two = ((BranchInstr *)instr)->getLabelTwo();
                        
                        Instruction *branch = new BranchInstr(cond, one, two, NULL);
                        IR2.push_back(branch);
                        break;
                    }

                }
                IR2.push_back(instr);
                break;
            }
            case RET:{
                Container cont = ((ReturnInstr *)instr)->getContainer();
                std::string reg;

                if (cont.isRegister()) {
                    reg = "%r" + std::to_string(cont.getIntValue());

                    //replace
                    if (constants.find(reg) != constants.end()) {
                        Container val = Container(IMM, constants[reg]);

                        Instruction *ret = new ReturnInstr(val, NULL);
                        IR2.push_back(ret);
                        break;
                    }
                }

                //add instr to new list
                IR2.push_back(instr);
                break;
            }

            default:
                IR2.push_back(instr);
                break;
        }
    }
    this->IR = IR2;
}

void Optimizer::constFold() {
    std::list<Instruction *> IR2;

    for(auto instr: this->IR) {
        switch(instr->getOp()) { 
            case BINARY_LET: {
                Container cont = ((LetInstr *)instr)->getOp1();
                Container cont2 = ((LetInstr *)instr)->getOp2();

                if (cont.isImmediate() && cont2.isImmediate()) {
                    int i = cont.getIntImmediate();
                    int j = cont2.getIntImmediate();
                    IrOp op = ((LetInstr *)instr)->getOperation();

                    Instruction *fin = new LetInstr(((LetInstr *)instr)->getContainer(), CONST_LET, NULL);

                    switch(op) {
                    case IrOp::ADD:
                        ((LetInstr *)fin)->setExpression(Container(IMM, i + j));
                        break;
                    case IrOp::SUB:
                        ((LetInstr *)fin)->setExpression(Container(IMM, i - j));
                        break;
                    case IrOp::MUL:
                        ((LetInstr *)fin)->setExpression(Container(IMM, i * j));
                        break;
                    case IrOp::DIV:
                        ((LetInstr *)fin)->setExpression(Container(IMM, i / j));
                        break;
                    default:
                        break;
                    }
                    IR2.push_back(fin);
                } else {
                    IR2.push_back(instr);
                }

                break;
            }
            default:
                IR2.push_back(instr);
                break;
        }
    }

    this->IR = IR2;
}

std::list<Instruction *> Optimizer::getIR() {
    return this->IR;
}

//print out the IR for debugging purposes
void Optimizer::output() {
    for(auto instr: this->IR) {
        std::cout << instr->toString() << std::endl;
    }
}