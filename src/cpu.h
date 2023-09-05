#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdbool.h>

#include "parser.h"
#include "bus.h"
#include "scoreboard.h"
#include "sys_bus.h"
#include "inst_buffer.h"

struct bus;
typedef struct bus bus;

struct sys_bus;
typedef struct sys_bus sys_bus;

typedef struct cpu
{
    int ck;
    int n_instructions;
    config cfg;

    bus *bus;
    sys_bus *sys_bus;

    inst_buffer *fetch_buffer;

    // para gerenciar parada no caso de aparecer um branch
    bool stall;
} cpu;

cpu *cpu_init(bus *b, sys_bus *sb, config cfg, int n_instructions);
void cpu_destroy(cpu *c);
void fetch(cpu *c, scoreboard *board);
void issue(cpu *c, scoreboard *board);
void read_operands(cpu *c, scoreboard *board);
void execution_complete(cpu *c, scoreboard *board);
void write_results(cpu *c, scoreboard *board);
bool pipeline(cpu *c);

#endif