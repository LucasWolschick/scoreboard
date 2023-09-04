#ifndef SCOREBOARDING_H
#define SCOREBOARDING_H

#include <stdint.h>
#include <stdbool.h>

#include "opcode.h"
#include "config.h"

typedef enum fu_type
{
    FU_ADD,
    FU_MUL,
    FU_INT,
} fu_type;

typedef struct uf_status
{
    bool busy;
    opcode op;
    fu_type type;

    // registers
    int fi;
    int fj;
    int fk;

    // fus
    int qj;
    int qk;

    bool rj;
    bool rk;
} uf_status;

typedef struct reg_status
{
    int uf;
} reg_status;

typedef enum stage
{
    STAGE_FETCH,
    STAGE_ISSUE,
    STAGE_READ_OPERANDS,
    STAGE_EXECUTION_COMPLETE,
    STAGE_WRITE_RESULTS,
    STAGE_DONE,
} stage;

typedef struct instruction_status
{
    // instrução
    uint32_t instruction;

    // uf
    int uf;

    // próximo estágio a ser executado
    stage st;

    // quando os estágios foram concluídos
    int when[STAGE_DONE + 1];
} instruction_status;

typedef struct scoreboard
{
    instruction_status *inst;
    reg_status *regs;
    uf_status *uf;
} scoreboard;

scoreboard *scoreboard_init(int n_registers, config *cfg);
void scoreboard_destroy(scoreboard *sb);

#endif