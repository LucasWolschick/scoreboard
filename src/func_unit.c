#include "func_unit.h"

void uf_load_instruction(uf *u, uint32_t instruction)
{
    u->instruction = instruction;
    u->res = 0;
}

void uf_load_ops(uf *u, bus *b, sys_bus *sb)
{
    // determina de onde ler
    int opcode = u->instruction >> 26;
    int rs, rt, imm;

    rs = (u->instruction >> 21) & 0x1F;
    rt = (u->instruction >> 16) & 0x1F;
    // rd = (u->instruction >> 11) & 0x1F;
    // extra = u->instruction & 0x7FF;
    imm = u->instruction & 0xFFFF;
    // address = u->instruction & 0x3FFFFFF;

    // assim que lê já executa
    switch (opcode)
    {
    case OP_ADD:
    {
        u->res = bus_read_reg(b, rs) + bus_read_reg(b, rt);
        break;
    }
    case OP_ADDI:
    {
        u->res = bus_read_reg(b, rs) + imm;
        break;
    }
    case OP_SUB:
    {
        u->res = bus_read_reg(b, rs) - bus_read_reg(b, rt);
        break;
    }
    case OP_SUBI:
    {
        u->res = bus_read_reg(b, rs) - imm;
        break;
    }
    case OP_MUL:
    {
        u->res = bus_read_reg(b, rs) * bus_read_reg(b, rt);
        break;
    }
    case OP_DIV:
    {
        u->res = bus_read_reg(b, rs) / bus_read_reg(b, rt);
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
        u->res = bus_read_reg(b, rs) < bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BGT:
    {
        u->res = bus_read_reg(b, rs) > bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BEQ:
    {
        u->res = bus_read_reg(b, rs) == bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_BNE:
    {
        u->res = bus_read_reg(b, rs) != bus_read_reg(b, rt);
        u->res2 = bus_read_pc(b) + 4 + imm;
        break;
    }
    case OP_J:
    {
        break;
    }
    case OP_LW:
    {
        u->res = sys_bus_read_memory(sb, bus_read_reg(b, rs) + imm);
        break;
    }
    case OP_SW:
    {
        u->res = bus_read_reg(b, rt);
        u->res2 = bus_read_reg(b, rs) + imm;
        break;
    }
    case OP_EXIT:
    {
        break;
    }
    }
}
void uf_write_res(uf *u, bus *b, sys_bus *sb)
{
    int opcode = u->instruction >> 26;
    int rt, rd, address;

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
        break;

    case OP_J:
        bus_write_pc(b, address);
        break;

    case OP_LW:
        bus_write_reg(b, rt, u->res);
        break;

    case OP_SW:
        sys_bus_write_memory(sb, u->res2, u->res);
        break;

    case OP_EXIT:
        bus_signal_exit(b);
        break;
    }
}