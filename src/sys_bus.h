#ifndef SYS_BUS_H
#define SYS_BUS_H

#include "memory.h"
#include "cpu.h"

struct cpu;
typedef struct cpu cpu;


/*
 * Struct que contém o barramento do sistema.
 */
typedef struct sys_bus
{
    memory *memory;
    cpu *cpu;
} sys_bus;

sys_bus *sys_bus_init(memory *m, cpu *c);
void sys_bus_destroy(sys_bus *b);

uint8_t sys_bus_read_memory(sys_bus *b, uint32_t address);
void sys_bus_write_memory(sys_bus *b, uint32_t address, uint8_t value);
uint32_t sys_bus_read_memory_word(sys_bus *b, uint32_t address);
void sys_bus_write_memory_word(sys_bus *b, uint32_t address, uint32_t value);

#endif