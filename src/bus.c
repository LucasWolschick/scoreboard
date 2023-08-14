#include "bus.h"

bus *bus_init(cpu *c, register_bank *r, memory *m, uf *f)
{
    bus *b = malloc(sizeof *b);
    b->cpu = c;
    b->registers = r;
    b->memory = m;
    b->func_units = f;
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

uint8_t bus_read_memory(bus *b, uint32_t address)
{
    return memory_read(b->memory, address);
}

void bus_write_memory(bus *b, uint32_t address, uint8_t value)
{
    memory_write(b->memory, address, value);
}

void bus_func_unit_load_instruction(bus *b, uint32_t unit, uint32_t instruction)
{
    uf_load_instruction(&b->func_units[unit], instruction);
}

void bus_func_unit_load_ops(bus *b, uint32_t unit)
{
    uf_load_ops(&b->func_units[unit], b);
}

void bus_func_unit_write_res(bus *b, uint32_t unit)
{
    uf_write_res(&b->func_units[unit], b);
}

void bus_signal_exit(bus *b)
{
    b->cpu->stop = true;
}
