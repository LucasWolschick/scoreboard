#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdbool.h>

#include "parser.h"
#include "bus.h"
#include "scoreboarding.h"

struct bus;
typedef struct bus bus;

typedef struct cpu
{
    int ck;
    int n_instructions;
    scoreboard_status *status;
    config cfg;

    bus *bus;

    // para gerenciar parada no caso de aparecer um branch
    bool stall;

    // para indicar que a instrução de parada foi executada
    bool stop;
} cpu;

cpu *cpu_init(bus *b, config cfg, int n_instructions);
void cpu_destroy(cpu *c);
void fetch(cpu *c);
void issue(cpu *c);
void read_operands(cpu *c);
void execution_complete(cpu *c);
void write_results(cpu *c);
void pipeline(cpu *c);

#endif