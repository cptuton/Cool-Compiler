/* File: compile.cpp
 * Authors: Christian
 * Description: Main executable for compilation
 * Todo:
 *  1. Add verbose option to make console output optional
 *  2. Implement parsing
 */

// Preprocessing
#include <iostream>
#include <istream>
#include <fstream>
#include <list>
#include <vector>

#include <unistd.h>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "pprinter.h"
#include "statement.h"
#include "symbol.h"
#include "symbolvisit.h"
#include "irep.h"
#include "irepvisit.h"
#include "exceptions.h"
#include "optIR.h"
#include "irparser.h"
#include "codegen.h"

// Functions
void printUsage();

// Program startn
int main(int argc, char *argv[]) {
    using namespace std;

    // Test number of input arguments
    if (argc < 2) {
        printUsage();
        exit(-1);
    }

    // Process commandline arguments
    int opt;
    int optLevel = 0;
    bool scanFlag = false;
    bool parseFlag = false;
    bool tableFlag = false;
    bool readIRFlag = false;
    bool irFlag = false;
    bool irOut = false;
    bool output = false;
    bool optFlag = false;

    std::string irFile = "";

    while ((opt = getopt(argc, argv, "sphtirO:I:o:")) != -1) {
        switch (opt) {
        // Stop at scanning
        case 's':
            scanFlag = true;
            break;
        // Stop at parsing
        case 'p':
            parseFlag = true;
            break;
        case 't':
            tableFlag = true;
            break;
        // Stop at IR gen
        case 'I':
            irFile = std::string(optarg);
            irOut =true;
            break;
        case 'i':
            irFlag = true;
            break;
        case 'r':
            readIRFlag = true;
            break;
        case 'o':
            irFile = std::string(optarg);
            output = true;
            break;
        case 'O':
            optFlag = true;
            optLevel = atoi(optarg);

            if (optLevel > 2 || optLevel <= 0) {
                std::cout << "Unsuported Optimization level. See usage." << std::endl;
                printUsage(); 
                exit(-1);
            }
            break;
        case 'h':
            printUsage();
            exit(-1);
            break;
        // Unused flag
        default:
            printUsage();
            exit(-1);
            break;
        }
    }

    // We output to out.s by default
    output = true;
    if (irFile == std::string("")) {
        irFile = std::string("out.s");
    }

    // Ensure there is file argument at the end of flags
    if (optind >= argc) {
        printUsage();
        exit(-1);
    }

    // Open file into string object
    string fileName = string(argv[optind]);
    ifstream file(fileName.data());
    if (!file.is_open()) {
        cerr << "No such file '" << fileName << "'\n";
        exit(-1);
    }
    string str((istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>());

    std::list<Instruction *> instructionList;

    // If we are reading in an IR file, then parse that, else
    // continue compilation normally
    if (readIRFlag == true) {
        IR_Parser irp(fileName);
        instructionList = irp.parse();
    } else {
        // -------------------------------------------------------
        // Scan
        // -------------------------------------------------------
        Scanner scanner(str);
        vector<Token> toks = scanner.lex();
        file.close();

        // Exit execution if scan flag has been set
        if (scanFlag == true) {
            for (auto i: toks) {
                cout << i << " " << endl;
            }
            return 1;
        }

        // Exit gracefully if the scanner encountered an error
        if (scanner.hadError()) {
            std::cerr << "Exiting with scanner error." << std::endl;
            exit(-1);
        }

        // -------------------------------------------------------
        // Parse
        // -------------------------------------------------------
        Parser parser(toks);
        std::list<Statement *> ast = parser.parse();

        if (parser.hadError()) {
            std::cerr << "Exiting with parser error." << std::endl;
            exit(-1);
        }

        // Print parse tree if parse flag is set
        if (parseFlag == true) {
            PrettyPrinter printer;

            for (std::list<Statement *>::iterator i = ast.begin(); i != ast.end(); ++i) {

                (*i)->accept(&printer);
            }

            freeAST(ast);
            return 1;
        }

        // -------------------------------------------------------
        // Generate symbol table
        // -------------------------------------------------------
        SymbolVisit visitor;

        for (std::list<Statement *>::iterator i = ast.begin(); i != ast.end(); ++i) {
            try {
                (*i)->accept(&visitor);
            } catch (SyntaxError &se) {
                visitor.thrownErr = true;
                se.printException();
            }
        }

        if (tableFlag == true) {
            std::cout << "===============\nVariable Tables\n===============\n\n";
            visitor.getGlobal()->printVarTable();
            std::cout << "===============\nFunction Tables\n===============\n\n";
            visitor.getGlobal()->printFunTable();

            
            return 1;
        }
            
        // Exit if table generator encountered syntax error
        if (visitor.thrownErr) {
            exit(-1);
        }
        
        // -------------------------------------------------------
        // Generate IR
        // -------------------------------------------------------

        // Give IR the Symbol Table
        IRepVisitor irepTransform(visitor.getGlobal());

        // Build the IR as you are going through the parse tree
        for (std::list<Statement *>::iterator i = ast.begin(); i != ast.end(); ++i) {
            (*i)->accept(&irepTransform);
        }
        // Check labels
        if (irepTransform.labelsAreGood() != true) {
            exit(-1); 
        }
        instructionList = irepTransform.getIR();
    }

    Optimizer optM = Optimizer(instructionList);
    optM.constProp();

    if (optFlag == true) {
        switch(optLevel) {
        case 1:
            optM.constFold();
            break;
        case 2:
            optM.constFold();
            optM.constProp();
            break;
        default:
            break;
        }
    }

    // separate IR into basic blocks
    std::vector<BasicBlock> bblist = bblist_of_instruction_list(optM.getIR());

    // remove empty basic blocks
    removeEmptyBlocks(bblist);

    // Print/Output IR if flag set
    if (irFlag == true) {
        int i = 0;
        for (auto &instr : bblist) {
            std::cout << "basic block " << i << std::endl;
            instr.iter([](Instruction *i) {
                std::cout << i->toString() << std::endl;
            });
            i++;
        }
        return 1;
    }

    CPU cpu = CPU();

    cpu.BuildRange(bblist);
    //used for debugging part of the register allocation
    // cpu.printRange();
    
    bblist = cpu.assignRegs(bblist);

    if (irOut == true) {
        ofstream outFile;
        outFile.open(irFile);

        for(auto i : optM.getIR()) {
            outFile << i->toString() << std::endl;
        }

        outFile.close();
    }

    Program p(fileName);
    bool hadError = false;

    for (auto &i : bblist) {
        try {
            p.insertBasicBlock(i);
        } catch (CodeGenError &ce) {
            ce.printException();
            hadError = true;
        }
    }

    if (hadError) {
        std::cout << "exiting with code generation error." << endl;
        exit(-1);
    }

    // std::cout << p.toString() << std::endl;

    if (output == true) {
        ofstream outFile;
        outFile.open(irFile);

        outFile << p.toString();

        outFile.close();
    }
    return 0;

}

// Prints the program use cases
void printUsage() {
    std::cout << "Usage: ./compile [-pstiIo] File\n"
        "-p: stop execution after parsing\n"
        "-s: stop execution after scanning\n"
        "-t: stop execution after filling symbol table\n"
        "-i: stop execution after IR generation\n"
        "-I [File]: stop execution after IR generation and output IR to [File]\n"
        "-O [opt level]: Optimize IR code, supported levels: 1-2\n"
        "-o [File] output to at end of compilation to [File]\n"
        "File: input file to be used\n";
}
