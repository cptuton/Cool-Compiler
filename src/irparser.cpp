#include <fstream>

#include "assert.h"
#include "irparser.h"

IR_Parser::IR_Parser(std::string path) {
    buildGrammar();

    irFilePath = path;
}

void IR_Parser::buildGrammar() {
    grammar = R"(
            # Grammar for IR
            Prog <- Stat+
            Stat <- Let / Def / Ret / VarDecl / If / Label / Jump / Call
            Call <- 'call' '@' ID '(' Arglist ')' ('->' Container)?
            If <- 'if' Container ID ID
            Label <- 'label' ID
            Jump <- 'jump' ID
            Def <- 'def' '@' ID '(' Paramlist ')' ('->' Var)?
            Ret <- 'return' Container / 'return'
            Let <- 'let' Container '=' Container Binop Container / 'let' Container '=' Container
            Var <- 'int' / 'float' / 'char'
            VarDecl <- 'decl' ID Var
            Arglist <- (Container (',' Container)*)?
            Paramlist <- (ID (',' ID)*)?
            Container <- Register / ID / < [0-9]+ >
            Register <- '%r' < [0-9]+ >
            ID <- < [a-zA-Z]+ >
            Binop <- '+' / '-' / '/' / '*'
            %whitespace <- [ \t\n]*
        )";

    parser.log = [](size_t line, size_t col, const std::string &msg) {
        throw IR_Error(std::string("[line ") + std::to_string(line) + ", " + std::to_string(col) + "] " + msg);
    };

    auto ok = parser.load_grammar(grammar.c_str());
    assert(ok);

    parser["Prog"] = [](const peg::SemanticValues &sv) {
        std::list<Instruction *> ilist;

        for (unsigned i = 0; i < sv.size(); i++) {
            ilist.push_back(sv[i].get<Instruction *>());
        }

        return ilist;
    };

    // Actions
    parser["Stat"] = [](const peg::SemanticValues &sv) -> Instruction * {
        switch (sv.choice()) {
            case 0:  // Let
                return sv[0].get<LetInstr *>();
            case 1: // Def
                return sv[0].get<DefInstr *>();
            case 2: // Ret
                return sv[0].get<ReturnInstr *>();
            case 3: // var declaration
                return sv[0].get<DeclInstr *>();
            case 4: // If
                return sv[0].get<BranchInstr *>();
            case 5: // Label
                return sv[0].get<LableInstr *>();
            case 6: // Jump
                return sv[0].get<JumpInstr *>();
            case 7: // Call
                return sv[0].get<CallInstr *>();
            default:
                throw IR_Error("Unhandled statement.");
        }
    };

    parser["Call"] = [](const peg::SemanticValues &sv) {
        return new CallInstr(sv[0].get<std::string>(), sv[1].get<std::list<Container>*>(), nullptr);
    };

    parser["Arglist"] = [](const peg::SemanticValues &sv) {
        std::list<Container> *alist = new std::list<Container>();

        for (unsigned i = 0; i < sv.size(); i++) {
            alist->push_back(sv[i].get<Container>());
        }

        return alist;
    };

    parser["Paramlist"] = [](const peg::SemanticValues &sv) {
        std::list<Container> *plist = new std::list<Container>();

        for (unsigned i = 0; i < sv.size(); i++) {
            std::string *s = new std::string(sv[i].get<std::string>());
            plist->push_back(Container(STR, s));
        }

        return plist;
    };

    parser["Jump"] = [](const peg::SemanticValues &sv) {
        return new JumpInstr(sv[0].get<std::string>(), nullptr);
    };

    parser["If"] = [](const peg::SemanticValues &sv) {
        Container cond = sv[0].get<Container>();
        std::string lbl1 = sv[1].get<std::string>();
        std::string lbl2 = sv[2].get<std::string>();

        return new BranchInstr(cond, lbl1, new std::string(lbl2), nullptr);
    };

    parser["Label"] = [](const peg::SemanticValues &sv) {
        return new LableInstr(sv[0].get<std::string>(), nullptr);
    };

    parser["Def"] = [](const peg::SemanticValues &sv) {
        return new DefInstr(sv[0].get<std::string>(), sv[2].get<SymbolType>(), sv[1].get<std::list<Container> *>(), nullptr);
    };

    parser["Ret"] = [](const peg::SemanticValues &sv) {
        switch (sv.choice()) {
            case 0:  // ret
                {
                    ReturnInstr *ri = new ReturnInstr(sv[0].get<Container>(), nullptr);
                    return ri;
                }
            case 1: // ret Container
                {
                    ReturnInstr *ri = new ReturnInstr(nullptr);
                    return ri;
                }
            default:
                throw IR_Error("Unhandled Return case.");
        }
    };

    // Handle variable type
    parser["Var"] = [](const peg::SemanticValues &sv) {
        switch (sv.choice()) {
            case 0: return SYM_INT;
            case 1: return SYM_FLOAT;
            case 2: return SYM_CHAR;
            default:
                    throw IR_Error("Unhandled Vartype syntax");
                    return SYM_ERR;
        }
    };

    parser["Let"] = [](const peg::SemanticValues &sv) {
        switch (sv.choice()) {
            case 0:  // let Container = Container 'Binop Container"
                {
                    LetInstr *li = new LetInstr(sv[0].get<Container>(), BINARY_LET, nullptr);
                    li->setExpression(sv[1].get<Container>(), sv[3].get<Container>(), sv[2].get<IrOp>());
                    return li;
                }
            case 1: // "let Container = Container"
                {
                    LetInstr *li = new LetInstr(sv[0].get<Container>(), CONST_LET, nullptr);
                    li->setExpression(sv[1].get<Container>());
                    return li;
                }
            default:
                throw IR_Error("Unhandled 'let' syntax.");
        }
    };

    parser["Container"] = [](const peg::SemanticValues &sv) {
        switch (sv.choice()) {
            case 0:  // register
                {
                    int reg = sv[0].get<int>();
                    Container c(REG, reg);
                    return c;
                }
            case 1: // id
                {
                    std::string *s = new std::string(sv[0].get<std::string>());
                    Container c(STR, s);
                    return c;
                    // return;
                }
            case 2: // immediate value
                {
                    int val = std::stoi(sv.token(), nullptr, 10);
                    Container c(IMM);
                    c.setVal(val);
                    return c;
                }
            default:
                throw IR_Error("Unhandled container type.");
        }
    };

    parser["Binop"] = [](const peg::SemanticValues &sv) {
        switch (sv.choice()) {
            case 0:  // +
                return IrOp::ADD;
            case 1: // -
                return IrOp::SUB;
            case 2: // /
                return IrOp::DIV;
            case 3: // *
                return IrOp::MUL;
            default:
                throw IR_Error("Unhandled binary operator.");
        }
    };

    parser["Register"] = [](const peg::SemanticValues& sv) {
        return std::stoi(sv.token(), nullptr, 10);
    };

    parser["ID"] = [](const peg::SemanticValues& sv) {
        return std::string(sv.token());
    };

    parser["VarDecl"] = [](const peg::SemanticValues& sv) {
        return new DeclInstr(new std::string(sv[0].get<std::string>()), sv[1].get<SymbolType>(), nullptr);
    };

    // enable packrat parsing
    parser.enable_packrat_parsing();
}

// Parse the file into a list of IR instructions
std::list<Instruction *>  IR_Parser::parse() {
    std::ifstream inFile(irFilePath);

    if (!inFile.is_open()) {
        throw IR_Error("Failed to open IR source file: " + irFilePath + ".");
    }

    std::string source((std::istreambuf_iterator<char>(inFile)),
                 std::istreambuf_iterator<char>());

    std::list<Instruction *> ilist;
    parser.parse(source.c_str(), ilist);

    return ilist;
}
