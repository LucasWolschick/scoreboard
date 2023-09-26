#include "opcode.h"

/*
 * string_from_opcode recebe um opcode e retorna uma string com o nome do opcode.
 */
const char *string_from_opcode(opcode op)
{
    switch (op)
    {
    case OP_ADD:
        return "add";
    case OP_ADDI:
        return "addi";
    case OP_SUB:
        return "sub";
    case OP_SUBI:
        return "subi";
    case OP_MUL:
        return "mul";
    case OP_DIV:
        return "div";
    case OP_AND:
        return "and";
    case OP_OR:
        return "or";
    case OP_NOT:
        return "not";
    case OP_BLT:
        return "blt";
    case OP_BGT:
        return "bgt";
    case OP_BEQ:
        return "beq";
    case OP_BNE:
        return "bne";
    case OP_J:
        return "j";
    case OP_LW:
        return "lw";
    case OP_SW:
        return "sw";
    case OP_EXIT:
        return "exit";
    default:
        return "";
    }
}