#include "irep.h"

class Optimizer {
private:
    std::list<Instruction *> IR;
public:
    Optimizer(std::list<Instruction *> IR);
    
    //function that handles constant propigation
    void constProp();
    void constFold();
    //retrieve IR
    std::list<Instruction *> getIR();
    //debuging function
    void output();
};