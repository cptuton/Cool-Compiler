/* File: pprinter.cpp
 * Authors: Nathan, Christian
 * Description: Overrides header statement.h and expression.h visitor class methods
 * With concrete ones meant to print the current data.
 */
 
#include <iostream>
#include "pprinter.h"

// StatementVisitor methods to override
void PrettyPrinter::visit(Statement &stat) {
    // Do nothing
}

// Visit Function Declaration statement
void PrettyPrinter::visit(FunctionDec &stat) {
    indent();
    std::cout << "Function Declaration " << stat.returnType.getLexeme() << " " << stat.name.getLexeme() << "( ";
    int count = 0;
    for(auto tn : stat.arglist) {
        if (count > 0)
            std::cout << ", ";
        std::cout << tn.first.getLexeme() << " " << tn.second.getLexeme(); 
        count++;
    }
    std::cout << " )" << std::endl;
}

// Visit Function Definition statement
void PrettyPrinter::visit(FunctionDef &stat) {
    indent();
    std::cout << "Function Definition " << std::endl;
    if (stat.fDec) {
        level++;
        stat.fDec->accept(this);
        level--;
    }
    if (stat.body) {
        level++;
        stat.body->accept(this);
        level--;
    }
}

// Visit Variable Declaration statement
void PrettyPrinter::visit(VarDec &stat) {
    indent();
    std::cout << "Variable Declaration " << stat.type.getLexeme() <<" " << stat.name.getLexeme() << std::endl; 
    if (stat.init) {
        level++;
        stat.init->accept(this);
        level--;
    }
}

// Visit Block statement
void PrettyPrinter::visit(Block &stat)  {
    indent();
    std::cout << "Block " << std::endl;
    for(auto stmt:stat.body) {
        level++;
        stmt->accept(this);
        level--;
    }
}

// Visit Return Statement
void PrettyPrinter::visit(Return &stat) {
    indent();
    std::cout << "Return " << std::endl;
    if (stat.retVal) {
        level++;
        stat.retVal->accept(this);
        level--;
    }
}

// Visit a while statement
void PrettyPrinter::visit(While &stat) {
    indent();
    std::cout << "while " << std::endl;
    if (stat.condition) {
        level++;
        stat.condition->accept(this);
        level--;
    }
    if (stat.body) {
        level++;
        stat.body->accept(this);
        level--;
    }
}

// Visit Expression Statment
void PrettyPrinter::visit(ExpStat &stat) {
    indent();
    std::cout << "Expression Statement " << std::endl;
    if (stat.exp) {
        level++;
        stat.exp->accept(this);
        level--;
    }
}

// Visit If statement
void PrettyPrinter::visit(If &stat) {
    indent();
    std::cout << "If" << std::endl;
    if (stat.condition) {
        level++;
        stat.condition->accept(this);
        level--;
    } if (stat.body) {
        level++;
        stat.body->accept(this);
        level--;
    } if (stat.elseBranch) {
        level++;
        stat.elseBranch->accept(this);
        level--;
    }
}

// Visit For statement
void PrettyPrinter::visit(For &stat) {
    indent();
    std::cout << "For " << std::endl;
    if (stat.init) {
        level++;
        stat.init->accept(this);
        level--;
    } if (stat.condition) {
        level++;
        stat.condition->accept(this);
        level--;
    } if (stat.inc) {
        level++;
        stat.inc->accept(this);
        level--;
    } if (stat.body) {
        level++;
        stat.body->accept(this);
        level--;
    }
}

// Visit Switch
void PrettyPrinter::visit(Switch &stat) {
    indent();
    std::cout << "Switch" << std::endl;
    if (stat.value) {
        level++;
        stat.value->accept(this);
        level--;
    }
    for (Statement *s : stat.body) {
        level++;
        s->accept(this);
        level--;
    }
}

// Visit Case
void PrettyPrinter::visit(Case &stat) {
    indent();
    std::cout << "Case " << std::endl;
    if (stat.value) {
        level++;
        stat.value->accept(this);
        level--;
    }

    for (Statement *s : stat.body) {
        level++;
        s->accept(this);
        level--;
    }
}

// Visit Goto
void PrettyPrinter::visit(Goto &stat) {
    indent();
    std::cout << "Goto " << stat.id.getLexeme() << std::endl;
}

// Visit Label
void PrettyPrinter::visit(Label &stat) {
    indent();
    std::cout << "Label " << stat.id.getLexeme() << std::endl;
}

// Visit break statement
void PrettyPrinter::visit(Break &stat) {
    indent();
    std::cout << "Break" << std::endl;
}

// ExpresisonVisitor methods to override
void PrettyPrinter::visit(Expression &exp) {
    // Do nothing
}

// Visit Binary expression
void PrettyPrinter::visit(BinaryExp &exp) {
    indent();
    std::cout << "Binary " << exp.op.getLexeme() << std::endl;
    if (exp.left) {
        level++;
        exp.left->accept(this);
        level--;
    }
    if (exp.right) {
        level++;
        exp.right->accept(this);
        level--;
    }
}

// Visit Unary expression
void PrettyPrinter::visit(UnaryExp &exp) {
    indent();
    std::cout << "Unary " << exp.op.getLexeme() << std::endl;
    if (exp.exp) {
        level++;
        exp.exp->accept(this);
        level--;
    }
}

// Primary expression
void PrettyPrinter::visit(PrimaryExp &exp) {
    indent();
    std::cout << "Primary " << exp.value.getLexeme() << std::endl;
}

// Visit Grouping expression
void PrettyPrinter::visit(GroupExp &exp) {
    indent();
    std::cout << "Group Expression" << std::endl;
    if (exp.exp) {
        level++;
        exp.exp->accept(this);
        level--;
    }
}

// Visit Function Call expression
void PrettyPrinter::visit(FunCall &exp) {
    indent();
    std::cout << "Function Call " << exp.name.getLexeme() << std::endl;
    for(auto expr:exp.arglist) {
        level++;
        expr->accept(this);
        level--;
    }
}

// Visit Assignment expression
void PrettyPrinter::visit(Assignment &exp) {
    indent();
    std::cout << "Assignment " << exp.name.getLexeme() << " " <<
        exp.op.getLexeme() << std::endl;
    if (exp.exp) {
        level++;
        exp.exp->accept(this);
        level--;
    }
}

// Indentation helper
void PrettyPrinter::indent() {
    for (int i = 0; i < level; i++) {
        std::cout << "|--";
    }
}
