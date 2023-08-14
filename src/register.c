#include "register.h"

register_bank *register_init(void)
{
    register_bank *r = malloc(sizeof(register_bank));
    r->pc = 0;
    r->ir = 0;
    for (int i = 0; i < N_REGISTERS; i++)
        r->regs[i] = 0;
    return r;
}

void register_destroy(register_bank *r)
{
    free(r);
}

void register_write(register_bank *r, uint8_t reg, uint32_t value)
{
    if (reg != 0)
        r->regs[reg] = value;
}

uint32_t register_read(register_bank *r, uint8_t reg)
{
    if (reg == 0)
        return 0;
    return r->regs[reg];
}

void register_write_pc(register_bank *r, uint32_t pc)
{
    r->pc = pc;
}

uint32_t register_read_pc(register_bank *r)
{
    return r->pc;
}

void register_write_ir(register_bank *r, uint32_t instruction)
{
    r->ir = instruction;
}

uint32_t register_read_ir(register_bank *r)
{
    return r->ir;
}