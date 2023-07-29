#ifndef INSTRUCAO
#define INSTRUCAO

#include <stdint.h>

enum opcode_instrucao
{
    OP_ADD = 0,
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
    OP_SW,

    OP_EXIT,
};
typedef enum opcode_instrucao opcode_instrucao;

struct inst_R
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int rd : 5;
    unsigned int extra : 11;
};

struct inst_I
{
    unsigned int opcode : 6;
    unsigned int rt : 5;
    unsigned int rs : 5;
    unsigned int imm : 16;
};

struct inst_J
{
    unsigned int opcode : 6;
    unsigned int address : 26;
};

struct inst
{
    union
    {
        uint32_t instrucao;
        struct inst_R r;
        struct inst_I i;
        struct inst_J j;
    };
};
typedef struct inst inst;

#endif
