#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdbool.h>

#include "parser.h"
#include "bus.h"
#include "scoreboard.h"
#include "sys_bus.h"

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

    // para gerenciar parada no caso de aparecer um branch
    bool stall;

    // para indicar que a instrução de parada foi executada
    bool stop;
} cpu;

cpu *cpu_init(bus *b, sys_bus *sb, config cfg, int n_instructions);
void cpu_destroy(cpu *c);
void fetch(cpu *c);
void issue(cpu *c);
void read_operands(cpu *c);
void execution_complete(cpu *c);
void write_results(cpu *c);
void pipeline(cpu *c);

#endif