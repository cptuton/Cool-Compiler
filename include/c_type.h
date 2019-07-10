#ifndef C_TYPE_H
#define C_TYPE_H

// Internal representation of a C value
// So far we can store ints chars and floats
union ctype {
    int ival;
    char cval;
    float fval;
};

// Type of symbol. VOID only for function return/parameter, NONE only for
//      function return value in Symbol class when variable is defined.
enum SymbolType {
    SYM_INT, SYM_CHAR, SYM_FLOAT, SYM_FUNCT, SYM_VOID, SYM_ERR = -1
};

#endif