#ifndef REGISTER_H
#define REGISTER_H

#include <stdlib.h>
#include <stdint.h>

#define N_REGISTERS 32

typedef struct register_bank
{
    uint32_t regs[N_REGISTERS];
    uint32_t pc;
    uint32_t ir;
} register_bank;

register_bank *register_init(void);
void register_destroy(register_bank *r);
void register_write(register_bank *r, uint8_t reg, uint32_t value);
uint32_t register_read(register_bank *r, uint8_t reg);
void register_write_pc(register_bank *r, uint32_t pc);
uint32_t register_read_pc(register_bank *r);
void register_write_ir(register_bank *r, uint32_t instruction);
uint32_t register_read_ir(register_bank *r);

#endif