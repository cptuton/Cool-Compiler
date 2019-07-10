/* File: symbolvisit.cpp
 *  Author: Christian
 *  Description: implementation of visitor pattern to traverse AST to check and
 *                   fill symbol table
 */

#include "symbolvisit.h"
//NOTE: Functions and variables cannot have the same name
//TODO: Make sure function not void when used in arithmetic expression

// StatementVisitor methods to override
void SymbolVisit::visit(Statement &stat) {
    // Do nothing
}

// Visit Function Declaration statement
void SymbolVisit::visit(FunctionDec &stat) {
    
    // Error test table insertion with pre-existing data
    Symbol *local = current->localFunLookup(stat.name.getLexeme());
    if (local != NULL) {
        if (local->hasDef) {
            // Definition already exists
            std::string err = "Redefinition of function '" +
                stat.name.getLexeme() + "'";
            throw SyntaxError(err, stat.name);

        } else {
            // Prototype already exists and trying to define prototype
            if (!visitingDef) {
                std::string err = "Redeclaration of function '" +
                    stat.name.getLexeme() + "'";
                throw SyntaxError(err, stat.name);

            } else {
                // Defining function definition with existing prototype
                
                // Add definition and setup next context for function
                local->hasDef = true;
                current->nextContext = stat.name.getLexeme();
                funVars = stat.arglist; // Keep track of names to add to next table


                // Check that parameters match
                if (local->getParameters().size() != stat.arglist.size()) {
                    std::string err = "Mismatching parameters in function '" +
                    stat.name.getLexeme() + "'";
                    throw SyntaxError(err, stat.name);

                }
            }
        }
    } else {
    // New entry
    
        funVars = stat.arglist; // Keep track of names to add to next table
        // Fill function parameter list
        std::vector<SymbolType> paramList;
        for(auto tn : stat.arglist) {
            paramList.push_back(symbolOfType(tn.first));
        }
        // Create symbol object
        Symbol f(SYM_FUNCT, symbolOfType(stat.returnType), paramList);
        
        // Place object into symbol table according to type
        if (visitingDef) {
            current->nextContext = stat.name.getLexeme();
            f.hasDef = true;
            f.hasProto = false;
        }
        else {
            f.hasProto = true;
            f.hasDef = false;
        }

        current->insertFun(stat.name.getLexeme(), f);
    }
}

// Visit Function Definition statement
void SymbolVisit::visit(FunctionDef &stat) {
    // Visit declaration
    visitingDef = true;
    
    if (stat.fDec) {
            stat.fDec->accept(this);
    }

    visitingDef = false; 
    visitingFunct = true;
    if (stat.body)
        stat.body->accept(this);
}

// Visit Variable Declaration statement
void SymbolVisit::visit(VarDec &stat) {
    // Check if variable already exists in local scope
    if (current->localVarLookup(stat.name.getLexeme()) != NULL) {
        std::string err = "Redeclaration of variable '" +
        stat.name.getLexeme() + "'";
        throw SyntaxError(err, stat.name);
    }
    
    Symbol s(symbolOfType(stat.type));
    current->insertVar(stat.name.getLexeme(), s);
    if (stat.init)
        stat.init->accept(this);
}

// Visit Block statement
void SymbolVisit::visit(Block &stat)  {
    // Create new scope, set as current
    current = current->addScope();
    
    // Add local variables to scope
    if (visitingFunct) {
        for (auto tn : funVars) {
            current->insertVar(tn.second.getLexeme(), symbolOfType(tn.first));
        }
        visitingFunct = false;
    }
    // Visit
    for (auto stmt : stat.body) {
        try {
            stmt->accept(this);
        } catch (SyntaxError &se) {
            thrownErr = true;
            se.printException();
        }
    }

    // Restore scope 
    current = current->getParent();
}

// Visit Return Statement
void SymbolVisit::visit(Return &stat) {
    Symbol* context = current->lookupFun(current->functionCtxt);
    if (stat.retVal) {
        // Make sure function isn't void
        if (context->getReturn() == SYM_VOID) {
            std::string err = "Mismatching return type for function '" +
            current->functionCtxt + "'";
            throw SyntaxError(err, stat.retTok);
        }
        stat.retVal->accept(this);
    } else if (context->getReturn() != SYM_VOID) {
        std::string err = "Returning variable in void function '" +
        current->functionCtxt + "'";
        throw SyntaxError(err, stat.retTok);

    }
}

// Visit a while statement
void SymbolVisit::visit(While &stat) {
    if (stat.condition)
        stat.condition->accept(this);
    if (stat.body)
        stat.body->accept(this);
}

// Visit Expression Statment
void SymbolVisit::visit(ExpStat &stat) {
    if (stat.exp)
        stat.exp->accept(this);
}

// Visit If statement
void SymbolVisit::visit(If &stat) {
    if (stat.condition)
        stat.condition->accept(this);
    if (stat.body)
        stat.body->accept(this);
    if (stat.elseBranch)
        stat.elseBranch->accept(this);
}

// Visit For statement
void SymbolVisit::visit(For &stat) {
    if (stat.init)
        stat.init->accept(this);
    if (stat.condition)
        stat.condition->accept(this);
    if (stat.inc)
        stat.inc->accept(this);
    if (stat.body)
        stat.body->accept(this);
}

// Visit Switch
void SymbolVisit::visit(Switch &stat) {
    stat.value->accept(this);
    for (Statement *s : stat.body) {
        s->accept(this);
    }
}

// Visit Case
void SymbolVisit::visit(Case &stat) {
    if (stat.value)
        stat.value->accept(this);

    for (Statement *s : stat.body) {
        s->accept(this);
    }
}

// Visit Goto
void SymbolVisit::visit(Goto &stat) {
}

// Visit Label
void SymbolVisit::visit(Label &stat) {
}

// Visit break statement
void SymbolVisit::visit(Break &stat) {
}

// ExpresisonVisitor methods to override
void SymbolVisit::visit(Expression &exp) {
    // Do nothing
}

// Visit Binary expression
void SymbolVisit::visit(BinaryExp &exp) {
    if (exp.left)
        exp.left->accept(this);
    if (exp.right)
        exp.right->accept(this);
}

// Visit Unary expression
void SymbolVisit::visit(UnaryExp &exp) {
    if (exp.exp)
        exp.exp->accept(this);
}

// Primary expression
void SymbolVisit::visit(PrimaryExp &exp) {

    // Test if identifier exists in symbol table
    if (exp.value.getType() == IDENTIFIER) {
        Symbol *s = current->lookupVar(exp.value.getLexeme());
        if (!s || s->isFunction()) {
            std::string err = "Undefined reference to variable '" +
            exp.value.getLexeme() + "'";
            throw SyntaxError(err, exp.value);
        }
    }
}

// Visit Grouping expression
void SymbolVisit::visit(GroupExp &exp) {
    if (exp.exp)
        exp.exp->accept(this);
}

// Visit Function Call expression
void SymbolVisit::visit(FunCall &exp) {
    // Test if function declaration exists in symbol table
    Symbol *s = current->lookupFun(exp.name.getLexeme());
    if (!s) {
        std::string err = "Function '" +
        exp.name.getLexeme() + "' does not exist in this context";
        throw SyntaxError(err, exp.name);
    }
    
   // Iterate over arguments and make sure size matches
    unsigned int argCount = 0;
    for(auto expr:exp.arglist) {
        expr->accept(this);
        argCount++;
    }
    
    // Throw error if the number of parameters != params given
    if ((s) && (argCount != s->getParameters().size())) {
        std::string err = "Function '" +
            exp.name.getLexeme() + "' does not exist with specified parameters"
            " in this context";
        throw SyntaxError(err, exp.name);
    }
}

// Visit Assignment expression
void SymbolVisit::visit(Assignment &exp) {
    if (!(current->lookupVar(exp.name.getLexeme()))) {
        std::string err = "Variable '" +
            exp.name.getLexeme() + "' does not exist in this context";
        throw SyntaxError(err, exp.name);
    }
    if(exp.exp) {
        exp.exp->accept(this);
    }
}

// Return pointer to current table/scope
SymbolTable* SymbolVisit::getCurrent() {
    return this->current;
}

// Constructor
SymbolVisit::SymbolVisit() {
    global = new SymbolTable(nullptr, "");

    current = global;
}

SymbolTable *SymbolVisit::getGlobal() {
    return global;
}
