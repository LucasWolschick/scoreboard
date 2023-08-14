#ifndef OPCODE_H
#define OPCODE_H

typedef enum opcode
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
    OP_SW,
    OP_EXIT
} opcode;

#endif