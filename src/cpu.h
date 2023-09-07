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

enum pipeline_status
{
    // executando normalmente
    PIPELINE_RUNNING,

    // busca interrompida
    PIPELINE_STALL,

    // busca sendo retomada
    PIPELINE_UNSTALL,

    // finalizou a execução
    PIPELINE_STOP
};
typedef enum pipeline_status pipeline_status;

typedef struct cpu
{
    int ck;
    int n_instructions;
    config cfg;

    bus *bus;
    sys_bus *sys_bus;

    inst_buffer *fetch_buffer;

    // para gerenciar parada no caso de aparecer um branch
    pipeline_status pipeline_status;
} cpu;

cpu *cpu_init(bus *b, sys_bus *sb, config cfg, int n_instructions);
void cpu_destroy(cpu *c);
void fetch(cpu *c, scoreboard *board);
void issue(cpu *c, scoreboard *board);
void read_operands(cpu *c, scoreboard *board);
void execution_complete(cpu *c, scoreboard *board);
void write_results(cpu *c, scoreboard *board);
bool pipeline(cpu *c);

void print_instruction(uint32_t instruction);

#endif