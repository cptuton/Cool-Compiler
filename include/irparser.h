#ifndef IR_PARSER_H
#define IR_PARSER_H

#include <list>

#include "exceptions.h"
#include "peglib.h"
#include "irep.h"

class IR_Parser {
private:
	std::string grammar;
	peg::parser parser;
    std::string irFilePath;
    std::string source;

    void buildGrammar();
public:
	IR_Parser(std::string path);

	// parse IR into IR structures
    std::list<Instruction *> parse();
};

#endif