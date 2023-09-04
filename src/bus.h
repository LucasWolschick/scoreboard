#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "register.h"
#include "memory.h"
#include "cpu.h"
#include "func_unit.h"
#include "scoreboard.h"
#include "sys_bus.h"

// conserta o probleminha
struct cpu;
typedef struct cpu cpu;

struct uf;
typedef struct uf uf;

struct scoreboard;
typedef struct scoreboard scoreboard;

typedef struct bus
{
    cpu *cpu;
    register_bank *registers;
    scoreboard *board;
    uf *func_units;
} bus;

bus *bus_init(cpu *c, register_bank *r, uf *f, scoreboard *sb);
void bus_destroy(bus *b);

uint32_t bus_read_pc(bus *b);
void bus_write_pc(bus *b, uint32_t pc);
uint32_t bus_read_ir(bus *b);
void bus_write_ir(bus *b, uint32_t instruction);
uint32_t bus_read_reg(bus *b, uint8_t reg);
void bus_write_reg(bus *b, uint8_t reg, uint32_t value);

void bus_func_unit_load_instruction(bus *b, uint32_t unit, uint32_t instruction);
void bus_func_unit_load_ops(bus *b, uint32_t unit, sys_bus *sb);
void bus_func_unit_write_res(bus *b, uint32_t unit, sys_bus *sb);

instruction_status *bus_sb_get_instruction_status(bus *b, uint32_t pc);
reg_status *bus_sb_get_register_status(bus *b, uint8_t reg);
uf_status *bus_sb_get_func_unit_status(bus *b, uint32_t unit);

void bus_signal_exit(bus *b);

#endif