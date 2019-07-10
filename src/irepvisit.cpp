/** File: irepvisit.cpp
 *  Author: Nathan, MiChAeL
 *  Description: implementation of visitor pattern to traverse the Parse Tree
 *  and create the IR 
 *  Problem : Currently not maintaing symbol table when we traverse AKA set nulls to real pointers if needed  
 */

#include "irepvisit.h"
#include "token.h"
// Helper functions
// Printer
void IRepVisitor::printIR() {
    // Print the entirety of the IR somehich Way
    for (auto instr: this->irep) {
        std::cout << instr->toString() << std::endl;    
    }
}

// Verifies if the goTo's match up with a label in scope
bool IRepVisitor::labelsAreGood() {
    // Label Names also have to be unique
    // The parser handles if they are in a scope or not but not for unique ness
    // Converting list of instructions to vector of instructions locally to check
    std::vector<Instruction *> goToListCheck;
    
    // Get the Label names and which function they occur in
    for (auto instr: this->irep) {
        goToListCheck.push_back(instr);
        if (instr->toString().find("label ") != std::string::npos) {
            this->labelNames.push_back(instr->toString());
            this->labelNames.back().erase(0,6);
            for (std::vector<Instruction *>::reverse_iterator rit=goToListCheck.rbegin(); rit!=goToListCheck.rend(); ++rit) {
                if ((*rit)->toString().find("def @") != std::string::npos) {
                    this->labelFunctionNames.push_back((*rit)->toString());
                    this->labelFunctionNames.back().erase(0,this->labelFunctionNames.back().find(" ") + 2);
                    this->labelFunctionNames.back().erase(this->labelFunctionNames.back().find("("),this->labelFunctionNames.back().npos);
                    break;
                }
            }
        }
    }
    
    // Now Check Label Unqiueness by comparing the name and the function it belongs too
    // If it is found that name and function name are duplicated then RETURN FALSE
    for (unsigned int i = 0; i < this->labelNames.size(); i++) {
        for (unsigned int j = i + 1; j < this->labelNames.size(); j++) {
            if ((this->labelNames.at(i).compare(this->labelNames.at(j)) == 0) & (this->labelFunctionNames.at(i).compare(this->labelFunctionNames.at(j)) == 0)) {
            // Need to  Throw propper error
                std::cout << "Error Non-Unique Labels: repeat of label: "<< this->labelNames.at(i) << " in function: "<<  this->labelFunctionNames.at(i) << std::endl;
                return false;
            }
        }
    }

    // Now we check every goto function against the label list
    for (unsigned int i = 0; i < this->goToLabelName.size(); i++) {
        // Check the function name of the goto to the function name of the labels
        // as well as the label names of each goto 
        for (unsigned int j = 0; j < this->labelNames.size(); j++) {
            // Same Function 
            if ((this->goToFuncName.at(i).compare(this->labelFunctionNames.at(j)) == 0) && (this->goToLabelName.at(i).compare(this->labelNames.at(j)) == 0)) {
                // Same Label
                break;
            }
            // if j = size - 1 then no match was found for this goto so report it and exit
            if (j == (this->labelNames.size() - 1)) {
                std::cout << "No Matching Label found for goto labelname: "<< this->goToLabelName.at(i) << " in scope of function: "<<  this->goToFuncName.at(i) << std::endl;
                return false;
            }
        }
    }
        
    // No errors so good
    return true;
}

// Generates a register variable name based on the current number of registers made
int IRepVisitor::generateReg() {
    int reg = this->currentReg;
    this->currentReg++;
    return reg;
}

// Generates a Label 
int IRepVisitor::generateLabel() {
    int label = this->currentLabel;
    this->currentLabel++;
    return label;
}

// Getter for current register
int IRepVisitor::getReg() {
    return this->currentReg;
}

std::list<Instruction *> IRepVisitor::getIR() {
    return this->irep;
}

// Constructor for setting the symbol table
// Subsequent function creations are each given their own symbol table with 0 being the global table

IRepVisitor::IRepVisitor(SymbolTable *table) {
    this->symbolTable = table;
}
IRepVisitor::IRepVisitor() {
}

// Helper to get symbol type
SymbolType IRepVisitor::getSymType(Token token) {
    switch(token.getType()) {
        case TYPE_INT: return  SYM_INT; break;
        case TYPE_CHAR: return  SYM_CHAR; break;
        case TYPE_FLOAT: return SYM_FLOAT; break;
        case TYPE_VOID: return SYM_VOID; break;
        default: return SYM_ERR;
    }
    
}
// StatementVisitor methods to override
void IRepVisitor::visit(Statement &stat) {
    // Do nothing
}


// Visit Function Declaration statement
void IRepVisitor::visit(FunctionDec &stat) {
    if (inFunc == 0) {
        return;
    }
    // Declarations are always a new scope so increase the scope
    //this->scope++;
    std::string funName = stat.name.getLexeme();
    this->funNameHolder = funName;
    //std::cout << stat.returnType.getLe << std::endl;
    SymbolType setType = this->getSymType(stat.returnType);
    
    
    // Build a list of container
    std::list<Container> *argList = new std::list<Container>(); 
    
    for(auto tn : stat.arglist) {
        std::string *argName = new std::string(tn.second.getLexeme() + "_1");
        
        Container piece = Container(STR, argName, this->getSymType(tn.first));
       
        argList->push_back(piece);
    }
    
    // Create Def Instr
    DefInstr *funDef = new DefInstr(funName, setType, argList, NULL);
    
    // Push it onto the list
    this->irep.push_back(funDef);
}

// Really just check the declaration and statement to visit them
void IRepVisitor::visit(FunctionDef &stat) {
    inFunc = 1;
    if (stat.fDec)
        stat.fDec->accept(this);
    inFunc = 0;
    if (stat.body)
        stat.body->accept(this);

    FuncEnd *end = new FuncEnd(NULL);
    this->irep.push_back(end);
}

// Visit Variable Declaration statement
void IRepVisitor::visit(VarDec &stat) {

    if (stat.init) {
        stat.init->accept(this);
    }
    
    // Var decs need to expect the primary isntruction which 
    //  which should be a primary expression blah blah'
    DeclInstr *varDecl = NULL;
    std::string scope = "_" +std::to_string(this->scope);
    std::string *varname = new std::string(stat.name.getLexeme() + scope);
    SymbolType symbol = this->getSymType(stat.type);
    varDecl = new DeclInstr(varname, symbol, NULL);
    
    // Check if there is any above function definition and throw a flag if there is
    int reDecFlag = 0;
    if (this->scope >= 1) {
        for (std::list<Instruction *>::reverse_iterator rit=this->irep.rbegin(); rit!=this->irep.rend(); ++rit) {
           // Check the definition
           
            if ((*rit)->toString().find("def @") != std::string::npos) {
                
                // ok we know we are at a definition instruction now
                DefInstr *copy = dynamic_cast<DefInstr *>((*rit));
                // Go throw the args list and compare the container names to the varname
                if (copy) {
                   // Get the parameter list
                   std::list<Container> *defParams = copy->getParams();
                   // Extract the name aka get rid of the _1 in params and compare to the first part of the declaration
                   std::string extract;
                   // Now check the names
                   for (auto container: *defParams) {
                        extract = container.toString();
                        extract.erase((extract.size() - 2), extract.npos);
                        //std::cout << container.toString() <<  " " << extract<<std::endl;
                        if (extract.compare(stat.name.getLexeme()) == 0) {
                            // Conflicting declarations
                            reDecFlag = 1;
                        }
                   }
                    
                } else {
                    std::cerr << "Dynamic Cast Did Not work "<<std::endl;
                    exit(-1);
                }
                
                // Break out on first function definition
                break; 
            }            
        } 
    }
    if (reDecFlag == 1) {
        std::cerr << "Variable Declaration: " << *varname << " already declared in parameters of function."<< std::endl;
        exit(-1);
    } else {
        this->irep.push_back(varDecl);
    }
    // Determine if it as assignment else this should be null
    if (stat.init) {
        Container varDeclCont = Container(STR, varname);
        Container prevReg = Container(REG, (this->getReg() - 1));
        
        LetInstr *assmt = new LetInstr(varDeclCont, CONST_LET, NULL);
        assmt->setExpression(prevReg);
        
        this->irep.push_back(assmt);
    }
    
    
}

// Visit Block statement
void IRepVisitor::visit(Block &stat)  {
    this->scope++;
    for(auto stmt:stat.body) {
        stmt->accept(this);
    }
    this->scope--;
    //std::cout << "AFTER:\n  current: " << this->current << "\n  tmp: " << tmp << "\n\n";
  
}

// Visit Return Statement
void IRepVisitor::visit(Return &stat) {
    ReturnInstr *rtrn = NULL;
    if (stat.retVal) {
        stat.retVal->accept(this);
        Container prevReg = Container(REG, (this->getReg() - 1));
        
        rtrn = new ReturnInstr(prevReg, NULL);
    } else {
        rtrn = new ReturnInstr(NULL);
    }
    
    // Create a return instruction as if you were making a a regular assignment
    this->irep.push_back(rtrn);
}

// Visit a while statement
void IRepVisitor::visit(While &stat) {
    // Create a label for the start
    std::string labelName = "L" + std::to_string(this->generateLabel());
    std::string *labelNameTwo = new std::string("L" + std::to_string(this->generateLabel()));
    LableInstr *whileLabelStart = new LableInstr(labelName, NULL); 
    LableInstr *whileLabelEnd = new LableInstr(*labelNameTwo, NULL);
    JumpInstr *jumpTooOne = new JumpInstr(labelName, NULL);
    //JumpInstr *jumpTooTwo = new JumpInstr(labelNameTwo, JUMP, NULL);
    
    this->irep.push_back(whileLabelStart);
    
    if (stat.condition)
        stat.condition->accept(this);
    Container prevReg = Container(REG, (this->getReg() - 1));
    BranchInstr *branchCode = new BranchInstr(prevReg, labelName, labelNameTwo, NULL);
    
    this->irep.push_back(branchCode);
        
    if (stat.body)
        stat.body->accept(this);

    this->irep.push_back(jumpTooOne);
    this->irep.push_back(whileLabelEnd);
}

// Visit Expression Statment
void IRepVisitor::visit(ExpStat &stat) {
    if (stat.exp)
        stat.exp->accept(this);
}

// Visit If statement
void IRepVisitor::visit(If &stat) {
    if (stat.condition)
        stat.condition->accept(this);
    
    Container condReg = Container(REG, (this->getReg() - 1));
    std::string labelName = "L" + std::to_string(this->generateLabel());
    std::string *labelNameTwo = NULL;
    
    LableInstr *label = new LableInstr(labelName, NULL);
    JumpInstr *jump = new JumpInstr(labelName, NULL);
    BranchInstr *branchCode = NULL;
    LableInstr *labelTwo = NULL;
    
    if (stat.elseBranch) {
            labelNameTwo = new std::string("L" + std::to_string(this->generateLabel()));
            labelTwo = new LableInstr(*labelNameTwo, NULL);
    } 

    branchCode = new BranchInstr(condReg, labelName, labelNameTwo, NULL);
    
    this->irep.push_back(branchCode);
    
   // Visit If body
    if (stat.body)
        stat.body->accept(this);
        

    
    if (stat.elseBranch) {
        this->irep.push_back(jump);
        this->irep.push_back(labelTwo);
        
        // Visit the else body
        stat.elseBranch->accept(this);
    }
    
    this->irep.push_back(label);
    
    
}

// Visit For statement
void IRepVisitor::visit(For &stat) {
    // This is initialization don't need to jump back to this code
    if (stat.init)
        stat.init->accept(this);
    
    // Main start of the branch instruction need to create the labels
    // Create a label for the start
    std::string labelName = "L" + std::to_string(this->generateLabel());
    std::string *labelNameTwo = new std::string("L" + std::to_string(this->generateLabel()));
    LableInstr *forLabelStart = new LableInstr(labelName, NULL); 
    LableInstr *forLabelEnd = new LableInstr(*labelNameTwo, NULL);
    JumpInstr *jumpTooOne = new JumpInstr(labelName, NULL);

    // Mark the start    
    this->irep.push_back(forLabelStart);
    
    // Check the conditional if not then jump too end
    if (stat.condition)
        stat.condition->accept(this);
        
    Container prevReg = Container(REG, (this->getReg() - 1));
    BranchInstr *branchCode = new BranchInstr(prevReg, labelName, labelNameTwo, NULL);
    
    this->irep.push_back(branchCode);
    
    // Visit the code and let it populate the stuff
    if (stat.body)
        stat.body->accept(this);
    
    // This should be the incrementer for the incrementing so this code needs to be put below the body
    if (stat.inc)
        stat.inc->accept(this);
    
    // Now we jump to the top again and mark the label for the end of the for code

    this->irep.push_back(jumpTooOne);
    this->irep.push_back(forLabelEnd);
}

// Visit Switch
void IRepVisitor::visit(Switch &stat) {
    stat.value->accept(this);
    // We need several things
    // The conditional 
    for (Statement *s : stat.body) {
        s->accept(this);
    }
}

// Visit Case
void IRepVisitor::visit(Case &stat) {
    if (stat.value)
        stat.value->accept(this);

    for (Statement *s : stat.body) {
        s->accept(this);
    }
}

// Visit Goto
void IRepVisitor::visit(Goto &stat) {
    std::string labelName = stat.id.getLexeme() + "_u";
    JumpInstr *jumpGoTo = new JumpInstr(labelName, NULL);
    
    this->irep.push_back(jumpGoTo);
    
    // Now push back the location of the goto jump statement which is just the current size of the instruciton list - 1 for 0 decrementing
    this->goToMarker.push_back(this->irep.size() - 1);
    
    // Check for the first definition instruction
    for (std::list<Instruction *>::reverse_iterator rit=this->irep.rbegin(); rit!=this->irep.rend(); ++rit) {
        if ((*rit)->toString().find("def @") != std::string::npos) {
            this->goToFuncName.push_back((*rit)->toString());
            this->goToFuncName.back().erase(0,this->goToFuncName.back().find(" ") + 2);
            this->goToFuncName.back().erase(this->goToFuncName.back().find("("),this->goToFuncName.back().npos);
            this->goToLabelName.push_back(labelName);
            break;
        }
    }

    
}

// Visit Label
void IRepVisitor::visit(Label &stat) {
    // Labels have ID's so push them onto the Structure
    std::string labelName = stat.id.getLexeme() + "_u";
    LableInstr *newLabel = new LableInstr(labelName, NULL);
    
    this->irep.push_back(newLabel);
}

// Visit break statement
void IRepVisitor::visit(Break &stat) {
}

// ExpresisonVisitor methods to override
void IRepVisitor::visit(Expression &exp) {
    // Do nothing
}

// Visit Binary expression
void IRepVisitor::visit(BinaryExp &exp) {
    if (exp.right)
        exp.right->accept(this);
    
    // Create a let expression to take both register - 1 and - 2
    Container prevRegRight = Container(REG, (this->getReg() - 1));

    if (exp.left)
        exp.left->accept(this);
    Container prevRegLeft = Container(REG, (this->getReg() - 1));
    
    // Now Create a let instr where the current register is assigned the previous two registers
    Container currentReg = Container(REG, this->generateReg());
    LetInstr *binInstr = new LetInstr(currentReg, BINARY_LET, NULL);
    binInstr->setExpression(prevRegLeft, prevRegRight, irop_of_token(exp.op));
    
    this->irep.push_back(binInstr);
}

// Visit assignment expression
void IRepVisitor::visit(Assignment &exp) {
    // Visit any below expressions which should translate the previous register to be the value needed
    if (exp.exp)
        exp.exp->accept(this);
    
    // Get previous register because at this point the assignment should be 
    // Variable name is %prevReg since generate reg function advances reg by 1
    // Each time it is called
    std::string *varName = NULL;// = new std::string( exp.name.getLexeme()+ "_"+ std::to_string(this->scope));
    Container varAssign;// = Container(STR, varName);
    Container prevReg = Container(REG, (this->getReg() - 1));
    // 
    // Check the list for any matching above declarations that match the 
    // Mark the exiting of the function 
    
    // Mark the when you exit a function and entering another
    //std::cout << exp.name.getLexeme() << std::endl;
    int ex = 0;
    int bad = 0;
    // This is the check loop that looks for any declarations in the function or in global
    // Sets a flag if it made through the entire list
    for (std::list<Instruction *>::reverse_iterator rit=this->irep.rbegin(); rit!=this->irep.rend(); ++rit) {
        if ((*rit)->toString().find("def @") != std::string::npos) {
            // Check the global list
            
            if (ex == 1) {
                varAssign = Container(STR, varName);
                break;
            }  
            DefInstr *copy = dynamic_cast<DefInstr *>((*rit));
            int paramFlag = 0;
            
            // Go throw the args list and compare the container names to the varname
            if (copy) {
               // Get the parameter list
               std::list<Container> *defParams = copy->getParams();
               // Extract the name aka get rid of the _1 in params and compare to the first part of the declaration
               std::string extract;
               // Now check the names
               for (auto container: *defParams) {
                    extract = container.toString();
                    extract.erase((extract.size() - 2), extract.npos);
                    //std::cout << container.toString() <<  " " << extract<<std::endl;
                    if (extract.compare(exp.name.getLexeme()) == 0) {
                        // Conflicting declarations
                        paramFlag = 1;
                    }
               }
               if (paramFlag == 1) {
                    varName = new std::string(exp.name.getLexeme() + "_1");
                    varAssign = Container(STR, varName);
                    break;
               }
               
                
            } else {
                std::cerr << "Dynamic Cast Did Not work "<<std::endl;
                exit(-1);
            }
            
             if (paramFlag == 0) {
                // Need to search the global list 
                unsigned int nearEnd = 0;
                for (std::list<Instruction *>::reverse_iterator star=this->irep.rbegin(); star!=this->irep.rend(); ++star) {
            
                        // If there was a declaration in the global scope and return 
                        if ((*star)->toString().find("decl " + exp.name.getLexeme() + "_0 ") != std::string::npos) {
                            varName = new std::string(exp.name.getLexeme() + "_0");
                            varAssign = Container(STR, varName);
                            break;
                        }
                        nearEnd++;
                    
                }
                //std::cout << nearEnd << "   " << this->irep.size() << std::endl;
                
                if (nearEnd == (this->irep.size())) {
                    // set bad flag
                    bad = 1;
                }
                break;
            }  
        }
        
        // You found a declaration that contains the string now check if they share the same name by extracting the _ number and if they are eaul then continue
        // Current logic error with finding names inside of names 
        if ((*rit)->toString().find("decl " + exp.name.getLexeme() ) != std::string::npos) {
            std::string varCompare = (*rit)->toString();
            std::string name;
            varCompare.erase(0,5);
            
            varCompare.erase(varCompare.find(" "), varCompare.npos);
            name = varCompare;
            name.erase((name.size() - 2),name.npos);
            //std::cout << name << " scope: " << this->scope <<std::endl;
            // Now compare if they are the same variable 
            if (name.compare(exp.name.getLexeme()) == 0) {
                //std::cout << varCompare << std::endl;
                std::string scopeExtract = varCompare;
                scopeExtract.erase(0 , varCompare.size() - 1);
                //std::cout << scopeExtract << std::endl;
                //std::cout << name << " scope: " << this->scope  << " | extracted scope: " << scopeExtract <<std::endl;
                //If the varnames are equal break else reassign the varname
                
                if (this->scope == std::stoi(scopeExtract)) {
                    varName = new std::string(varCompare);
                    varAssign = Container(STR, varName);
                    break;
                } else if (this->scope > std::stoi(scopeExtract)) {
                    // Record the closest visible scope
                    if (varName == NULL) {
                        varName = new std::string(varCompare);
                    }

                    ex = 1;
                }
            }
        }
    }
    

    // Check the exit flag meaning you are out of the function
    if (bad == 1) {
        std::cerr << "Error No Declaration for variable: " << exp.name.getLexeme() << std::endl;
        exit(-1);
    }
    
    // Switch over the different type of assignments
    // All of these do something and push that something to a register and then assig x to the previous register
    switch (exp.op.getType()) {
        case PLUS_EQUAL: {
            // Have to advance the register for assignment and let the instructions push in a specific order 
            Container currentReg = Container(REG, this->generateReg());
            LetInstr *plusEqual = new LetInstr(currentReg, BINARY_LET, NULL);
            plusEqual->setExpression(varAssign, prevReg, IrOp::ADD);
            LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
            assignment->setExpression(currentReg);
            
            //Push let instructions onto the board
            this->irep.push_back(plusEqual);
            this->irep.push_back(assignment);
            break; 
       }
       case MINUS_EQUAL: {
            // Have to advance the register for assignment and let the instructions push in a specific order 
            Container currentReg = Container(REG, this->generateReg());
            LetInstr *minusEqual = new LetInstr(currentReg, BINARY_LET, NULL);
            minusEqual->setExpression(varAssign, prevReg, IrOp::SUB);
            LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
            assignment->setExpression(currentReg);
            
            //Push let instructions onto the board
            this->irep.push_back(minusEqual);
            this->irep.push_back(assignment);
            break; 
       }
       case SLASH_EQUAL:{
            // Have to advance the register for assignment and let the instructions push in a specific order 
            Container currentReg = Container(REG, this->generateReg());
            LetInstr *slashEqual = new LetInstr(currentReg, BINARY_LET, NULL);
            slashEqual->setExpression(varAssign, prevReg, IrOp::DIV);
            LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
            assignment->setExpression(currentReg);
            
            //Push let instructions onto the board
            this->irep.push_back(slashEqual);
            this->irep.push_back(assignment);
            break; 
       }
       case STAR_EQUAL: {
            // Have to advance the register for assignment and let the instructions push in a specific order 
            Container currentReg = Container(REG, this->generateReg());
            LetInstr *starEqual = new LetInstr(currentReg, BINARY_LET, NULL);
            starEqual->setExpression(varAssign, prevReg,  IrOp::MUL);
            LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
            assignment->setExpression(currentReg);
            
            //Push let instructions onto the board
            this->irep.push_back(starEqual);
            this->irep.push_back(assignment);
            break; 
       }
       case MOD_EQUAL: {
            // Have to advance the register for assignment and let the instructions push in a specific order 
            Container currentReg = Container(REG, this->generateReg());
            LetInstr *modEqual = new LetInstr(currentReg, BINARY_LET, NULL);
            modEqual->setExpression(varAssign, prevReg, IrOp::MOD);
            LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
            assignment->setExpression(currentReg);
            
            //Push let instructions onto the board
            this->irep.push_back(modEqual);
            this->irep.push_back(assignment);
            break; 
       }
       // Just an equals
       default: 
        LetInstr *assignment = new LetInstr(varAssign, CONST_LET, NULL);
        assignment->setExpression(prevReg);
        this->irep.push_back(assignment);
    }
}

// Visit Unary expression
void IRepVisitor::visit(UnaryExp &exp) {
    if (exp.exp)
        exp.exp->accept(this);
    // At this point it should be the previous register that we need to apply 
    //  The unary expression too since its unaryOp %prevReg
    Container prevReg = Container(REG, (this->getReg() - 1));
    Container storeReg= Container(REG, this->generateReg());
    LetInstr *unaryInstr = new LetInstr(storeReg, UNARY_LET, NULL);
    unaryInstr->setExpression(prevReg, irop_of_token(exp.op));
    
    this->irep.push_back(unaryInstr);
    
    // Need to reassign the the expression to the previous register or have the error handeled in parser
}

// Primary expression
void IRepVisitor::visit(PrimaryExp &exp) {
    switch (exp.value.getType()) {
        case IDENTIFIER: {
            std::string *varName = NULL;// = new std::string(exp.value.getLexeme());
            Container idCont ; // = Container(STR, idName);
            // Check to see which declaration you are using
            int ex = 0;
            int bad = 0;
            for (std::list<Instruction *>::reverse_iterator rit=this->irep.rbegin(); rit!=this->irep.rend(); ++rit) {
               // Check the global list
                if ((*rit)->toString().find("def @") != std::string::npos) {

                    // ok we know we are at a definition instruction now
                    // WE need to check the argslist then the global list unless it was already found
                    if (ex == 1) {
                        idCont = Container(STR, varName);
                        break;
                    }
                    
                    DefInstr *copy = dynamic_cast<DefInstr *>((*rit));
                    int paramFlag = 0;
                    
                    // Go throw the args list and compare the container names to the varname
                    if (copy) {
                       // Get the parameter list
                       std::list<Container> *defParams = copy->getParams();
                       // Extract the name aka get rid of the _1 in params and compare to the first part of the declaration
                       std::string extract;
                       // Now check the names
                       for (auto container: *defParams) {
                            extract = container.toString();
                            extract.erase((extract.size() - 2), extract.npos);
                            //std::cout << container.toString() <<  " " << extract<<std::endl;
                            if (extract.compare(exp.value.getLexeme()) == 0) {
                                // Conflicting declarations
                                paramFlag = 1;
                            }
                       }
                       if (paramFlag == 1) {
                            varName = new std::string(exp.value.getLexeme() + "_1");
                            idCont = Container(STR, varName);
                            break;
                       }
                       
                        
                    } else {
                        std::cerr << "Dynamic Cast Did Not work "<<std::endl;
                        exit(-1);
                    }
                    
                     if (paramFlag == 0) {
                        // Need to search the global list 
                        unsigned int nearEnd = 0;
                        for (std::list<Instruction *>::reverse_iterator star=this->irep.rbegin(); star!=this->irep.rend(); ++star) {
                    
                                // If there was a declaration in the global scope and return 
                                if ((*star)->toString().find("decl " + exp.value.getLexeme() + "_0 ") != std::string::npos) {
                                    varName = new std::string(exp.value.getLexeme() + "_0");
                                    idCont = Container(STR, varName);
                                    break;
                                }
                                nearEnd++;
                            
                        }
                        //std::cout << nearEnd << "   " << this->irep.size() << std::endl;
                        
                        if (nearEnd == (this->irep.size())) {
                            // set bad flag
                            bad = 1;
                        }
                        break;
                    }  
                }
                
                // You found a declaration that contains the string now check 
                // if they share the same name by extracting the _ number and if they are equal then continue
                if ((*rit)->toString().find("decl " + exp.value.getLexeme() ) != std::string::npos) {
                    std::string varCompare = (*rit)->toString();
                    std::string name;
                    varCompare.erase(0,5);
                    
                    varCompare.erase(varCompare.find(" "), varCompare.npos);
                    name = varCompare;
                    name.erase((name.size() - 2),name.npos);
                    //std::cout << name << " scope: " << this->scope <<std::endl;
                    // Now compare if they are the same variable 
                    if (name.compare(exp.value.getLexeme()) == 0) {
                        //std::cout << varCompare << std::endl;
                        std::string scopeExtract = varCompare;
                        scopeExtract.erase(0 , varCompare.size() - 1);
                        //std::cout << scopeExtract << std::endl;
                        //std::cout << name << " scope: " << this->scope  << " | extracted scope: " << scopeExtract <<std::endl;
                        //If the varnames are equal break else reassign the varname
                        
                        if (this->scope == std::stoi(scopeExtract)) {
                            varName = new std::string(varCompare);
                            idCont = Container(STR, varName);
                            break;
                        } else if (this->scope > std::stoi(scopeExtract)) {
                            // Record the closest visible scope
                            if (varName == NULL) {
                                varName = new std::string(varCompare);
                            }

                            ex = 1;
                        }
                    }
                }
            }
            

            // Check the exit flag meaning you are out of the function
            if (bad == 1) {
                std::cerr << "Error No Declaration for variable call: " << exp.value.getLexeme() << std::endl;
                exit(-1);
            }
             
            Container regCont = Container(REG, this->generateReg());

            
            LetInstr *letVar = new LetInstr(regCont, CONST_LET, NULL);
            letVar->setExpression(idCont);
            
            this->irep.push_back(letVar);
            break;
        }
        case INTEGER: {
            Container regCont = Container(REG, this->generateReg());
           // std::cout << regCont.toString() << std::endl;
            // Currently the set getLiteral function is not working so using stoi
            // Will fix implementation later
            //regCont.setVal(exp.value.getLiteral<int>());
            // Immediate container 
            Container imdCont = Container(IMM);
            imdCont.setVal(std::stoi(exp.value.getLexeme()));
            
            LetInstr *letReg = new LetInstr(regCont, CONST_LET, NULL);
            letReg->setExpression(imdCont);
            //std::cout << letReg->toString() << std::endl;

            this->irep.push_back(letReg);
                //        std::cout << "I pushed an instruction"<< std::endl;
            break;
        }
        default: {   
            // Throw Exception
           // std::cout << "I am in a thrown exception"<< std::endl;
        }
    }
}

// Visit Grouping expression
void IRepVisitor::visit(GroupExp &exp) {
    if (exp.exp)
        exp.exp->accept(this);
}

// Visit Function Call expression
void IRepVisitor::visit(FunCall &exp) {
    
    std::string funName = exp.name.getLexeme();
    //std::string *funNamePtr = new std::string(exp.name.getLexeme());
    std::list<Container> *args = new std::list<Container>();
    
    for(auto expr:exp.arglist) {
        expr->accept(this); 
        Container prevReg = Container(REG, (this->getReg() - 1));
        args->push_back(prevReg);
    }
    // Get the return type of the function name 
    //CallInstr *funCall = new CallInstr(funName, args, NULL);
    //std::cout << this->symbolTable->lookup(funName)->getReturn()  << std::endl;
    
    if (this->symbolTable->lookupFun(funName)->getReturn() == SYM_VOID) {
            CallInstr *funCall = new CallInstr(funName, args, NULL);
            this->irep.push_back(funCall);
    } else {
        Container currentReg = Container(REG, this->generateReg());
        CallInstr *funCall = new CallInstr(funName, args, NULL);
        funCall->setExpression(currentReg);
        this->irep.push_back(funCall);
    }
    
    // Capture output in register and push it onto the instruction list so that whatever called it can use its value
    
/*
    Container callName = Container(STR, funNamePtr);
    
    LetInstr *funCallCapture = new LetInstr(currentReg, CONST_LET, NULL);
    funCallCapture->setExpression(callName);
    
    this->irep.push_back(funCallCapture);
    */
}

