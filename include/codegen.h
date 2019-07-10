/** File: codegen.h
 *  Authors: Christian, Elias
 *  Description: Defines structures and functions for the transition of IR into x86
 *  Note: Description of IR to assembly construction at bottom
 */
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "irep.h"

enum Datalen {DCHAR = 1, DSHORT = 2, DINT = 4, DLONG = 8};

class CPU {
    private:
        //vector containing currently free registers
        std::vector<std::string> freeRegStack;
        //map containing vr as a key and the range that the vr is valid
        std::map<std::string, std::pair<int, int>> regRange;
        //maps currently allocated virtual registers to physical ones
        std::map<std::string, std::string> vrToPr;
        //maps phyisical registers to free status
        std::map<std::string, bool> usage;
        
    public:
        CPU();
        void BuildRange(std::vector<BasicBlock> &bblist);
        void freeVar(int index);

        std::string Allocate(std::string vr);
        std::string Ensure(std::string vr);
        //replace virtual registers with physical registers
        std::vector<BasicBlock> assignRegs(std::vector<BasicBlock> &bblist);
        void Free(std::string reg);
        void printRange(); //debugging
};

class Program {
private:
    int stack_level;
    // Each bool represents 4 bytes taken
    std::vector<int> stack;
    //map containig variable names as key, and he corresponding stack offset
    std::vector<std::map<std::string, int>> offsets;
    std::string file;
    std::string data;
    std::string bss;
    std::string text;
    CPU cpu;

    bool inFunction;
    int numArgs;

    std::vector<std::string> globals;

    void declareFunction(std::string name);
    void declareGlobal(std::string name);
    int allocVar(std::string var);
    void newFunc();
    int offset(int index);
    bool findGlobal(std::string name);
    std::string toAssembly(LetInstr *li);
    std::string toAssembly(Container c);
public:
    Program(std::string fileName);

    std::string toString();

    void insertBasicBlock(BasicBlock &bb);
};

void genTest();

/* IR op to Assembly guide:

turn let x = y (&&,||) into:
----------------------------
    cmp $0, y
    j(n)e  .L1
    cmp $0, z
    j(n)e  .L1
    mov $1, x
    jmp .L2
.L1:
    mov $0, x
.L2

turn let x = y op z into:
-------------------------
    mov y, x
    op  z, x

turn let x = y (<,<=,>,>=) z into:
----------------------------------
    mov y, x
    sub z, x
    j__ .BL1
    mov $0, x
    jmp .BL2
.BL1:
    mov $1, x
.BL2:

*/
