#include "bus.h"

bus *bus_init(cpu *c, register_bank *r, uf *f, scoreboard *sb)
{
    bus *b = malloc(sizeof *b);
    b->cpu = c;
    b->registers = r;
    b->func_units = f;
    b->board = sb;
    return b;
}

void bus_destroy(bus *b)
{
    free(b);
}

void bus_write_pc(bus *b, uint32_t pc)
{
    register_write_pc(b->registers, pc);
}

uint32_t bus_read_pc(bus *b)
{
    return register_read_pc(b->registers);
}

uint32_t bus_read_ir(bus *b)
{
    return register_read_ir(b->registers);
}

void bus_write_ir(bus *b, uint32_t instruction)
{
    register_write_ir(b->registers, instruction);
}

uint32_t bus_read_reg(bus *b, uint8_t reg)
{
    return register_read(b->registers, reg);
}

void bus_write_reg(bus *b, uint8_t reg, uint32_t value)
{
    register_write(b->registers, reg, value);
}

void bus_func_unit_load_instruction(bus *b, uint32_t unit, uint32_t instruction)
{
    uf_load_instruction(&b->func_units[unit], instruction);
}

void bus_func_unit_load_ops(bus *b, uint32_t unit, sys_bus *sb)
{
    uf_load_ops(&b->func_units[unit], b, sb);
}

void bus_func_unit_write_res(bus *b, uint32_t unit, sys_bus *sb)
{
    uf_write_res(&b->func_units[unit], b, sb);
}

void bus_signal_exit(bus *b)
{
    b->cpu->stop = true;
}

reg_status *bus_sb_get_register_status(bus *b, uint8_t reg)
{
    return &b->board->regs[reg];
}

uf_status *bus_sb_get_func_unit_status(bus *b, uint32_t unit)
{
    return &b->board->uf[unit];
}

uint32_t bus_sb_add_instruction(bus *b, instruction_status is)
{
    scoreboard_add_instruction(b->board, is);
}

instruction_status *bus_sb_get_instruction(bus *b, int iid)
{
    return scoreboard_get_instruction(b->board, iid);
}

size_t bus_sb_n_instructions(bus *b)
{
    return scoreboard_n_instructions(b->board);
}