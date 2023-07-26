#ifndef INSTRUCAO
#define INSTRUCAO

enum opcode_instrucao
{
    OP_ADD,
    OP_ADDI,
    OP_SUB,
    OP_SUBI,
    OP_MUL,
    OP_DIV,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_BLT,
    OP_BGT,
    OP_BEQ,
    OP_BNE,
    OP_J,
    OP_LW,
    OP_SW
};
typedef enum opcode_instrucao opcode_instrucao;

typedef struct inst inst;
struct inst
{
    opcode_instrucao opcode;
    int op1;
    int op2;
    int op3;
};

#endif
