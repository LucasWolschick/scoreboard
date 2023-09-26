#include "func_unit.h"


/*
 * uf_load_instruction recebe uma unidade funcional e uma instrução, com isso,
 * carrega uma instrução na unidade funcional.
 */
void uf_load_instruction(uf *u, uint32_t instruction)
{
    u->instruction = instruction;
    u->res = 0;
}

/*
 * uf_load_ops recebe um struct bus, que contém o barramento, um struct sys_bus, que 
 * contém o barramento do sistema e uma unidade funcional, com isso, carrega os operandos 
 * das unidades funcionais.
 */
void uf_load_ops(uf *u, bus *b, sys_bus *sb)
{
    // determina de onde ler
    uint32_t opcode = u->instruction >> 26;
    uint32_t rs, rt, imm;

    rs = (u->instruction >> 21) & 0x1F;
    rt = (u->instruction >> 16) & 0x1F;
    // rd = (u->instruction >> 11) & 0x1F;
    // extra = u->instruction & 0x7FF;
    imm = u->instruction & 0xFFFF;
    if (imm & 0x8000)
    {
        imm |= 0xFFFF0000;
    }
    // address = u->instruction & 0x3FFFFFF;

    // assim que lê já executa
    switch (opcode)
    {
    case OP_ADD:
    {
        u->res = (int32_t)bus_read_reg(b, rs) + (int32_t)bus_read_reg(b, rt);
        break;
    }
    case OP_ADDI:
    {
        u->res = (int32_t)bus_read_reg(b, rs) + imm;
        break;
    }
    case OP_SUB:
    {
        u->res = (int32_t)bus_read_reg(b, rs) - (int32_t)bus_read_reg(b, rt);
        break;
    }
    case OP_SUBI:
    {
        u->res = (int32_t)bus_read_reg(b, rs) - imm;
        break;
    }
    case OP_MUL:
    {
        u->res = (int32_t)bus_read_reg(b, rs) * (int32_t)bus_read_reg(b, rt);
        break;
    }
    case OP_DIV:
    {
        int32_t divisor = (int32_t)bus_read_reg(b, rt);
        if (divisor == 0)
        {
            fprintf(stderr, "Erro: divisao por zero! Abortando\n");
            exit(1);
        }
        u->res = (int32_t)bus_read_reg(b, rs) / divisor;
        break;
    }
    case OP_AND:
    {
        u->res = bus_read_reg(b, rs) & bus_read_reg(b, rt);
        break;
    }
    case OP_OR:
    {
        u->res = bus_read_reg(b, rs) | bus_read_reg(b, rt);
        break;
    }
    case OP_NOT:
    {
        u->res = ~bus_read_reg(b, rs);
        break;
    }
    case OP_BLT:
    {
        u->res = (int32_t)bus_read_reg(b, rs) < (int32_t)bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BGT:
    {
        u->res = (int32_t)bus_read_reg(b, rs) > (int32_t)bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BEQ:
    {
        u->res = (int32_t)bus_read_reg(b, rs) == (int32_t)bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BNE:
    {
        u->res = (int32_t)bus_read_reg(b, rs) != (int32_t)bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_J:
    {
        break;
    }
    case OP_LW:
    {
        size_t offset = (int32_t)bus_read_reg(b, rs) + imm;
        uint32_t res = sys_bus_read_memory_word(sb, offset);
        u->res = res;
        break;
    }
    case OP_SW:
    {
        u->res = (int32_t)bus_read_reg(b, rt);
        u->res2 = (int32_t)bus_read_reg(b, rs) + imm;
        break;
    }
    case OP_EXIT:
    {
        break;
    }
    }
}

/*
 * uf_write_res recebe um struct bus, que contém o barramento, um struct sys_bus, que
 * contém o barramento do sitema, e uma unidade funcional, tendo como objetivo, escrever
 * os resultados das unidades funcionais.
 */
void uf_write_res(uf *u, bus *b, sys_bus *sb)
{
    uint32_t opcode = u->instruction >> 26;
    uint32_t rt, rd, address;

    // rs = (u->instruction >> 21) & 0x1F;
    rt = (u->instruction >> 16) & 0x1F;
    rd = (u->instruction >> 11) & 0x1F;
    // extra = u->instruction & 0x7FF;
    // imm = u->instruction & 0xFFFF;
    address = u->instruction & 0x3FFFFFF;

    switch (opcode)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_AND:
    case OP_OR:
    case OP_NOT:
        bus_write_reg(b, rd, u->res);
        break;

    case OP_ADDI:
    case OP_SUBI:
        bus_write_reg(b, rt, u->res);
        break;

    case OP_BLT:
    case OP_BGT:
    case OP_BEQ:
    case OP_BNE:
        if (u->res)
        {
            bus_write_pc(b, u->res2);
        }
        else
        {
            bus_write_pc(b, bus_read_pc(b) + 4);
        }
        break;

    case OP_J:
        bus_write_pc(b, address);
        break;

    case OP_LW:
        bus_write_reg(b, rt, u->res);
        break;

    case OP_SW:
        sys_bus_write_memory_word(sb, u->res2, u->res);
        break;

    case OP_EXIT:
        break;
    }
}