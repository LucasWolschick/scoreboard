#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "register.h"
#include "memory.h"
#include "cpu.h"
#include "func_unit.h"

// conserta o probleminha
struct cpu;
typedef struct cpu cpu;

struct uf;
typedef struct uf uf;

typedef struct bus
{
    cpu *cpu;
    register_bank *registers;
    memory *memory;
    uf *func_units;
} bus;

bus *bus_init(cpu *c, register_bank *r, memory *m, uf *f);
void bus_destroy(bus *b);

uint32_t bus_read_pc(bus *b);
void bus_write_pc(bus *b, uint32_t pc);
uint32_t bus_read_ir(bus *b);
void bus_write_ir(bus *b, uint32_t instruction);
uint32_t bus_read_reg(bus *b, uint8_t reg);
void bus_write_reg(bus *b, uint8_t reg, uint32_t value);

uint8_t bus_read_memory(bus *b, uint32_t address);
void bus_write_memory(bus *b, uint32_t address, uint8_t value);

void bus_func_unit_load_instruction(bus *b, uint32_t unit, uint32_t instruction);
void bus_func_unit_load_ops(bus *b, uint32_t unit);
void bus_func_unit_write_res(bus *b, uint32_t unit);

void bus_signal_exit(bus *b);

#endif