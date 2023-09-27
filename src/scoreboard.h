#ifndef SCOREBOARDING_H
#define SCOREBOARDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "opcode.h"
#include "config.h"


/*
 * Enum que contém os tipos de unidades funcionais.
 */
typedef enum fu_type
{
    FU_ADD,
    FU_MUL,
    FU_INT,
} fu_type;


/*
 * Struct que contém as informações (status) de uma unidade funcional.
 */
typedef struct uf_status
{
    bool busy;
    opcode op;
    fu_type type;
    int n_type;

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


/*
 * Struct que contém as informações (status) de um registrador.
 */
typedef struct reg_status
{
    int uf;
} reg_status;


/*
 * Enum que contém os estágios de uma instrução.
 */
typedef enum stage
{
    STAGE_FETCH,
    STAGE_ISSUE,
    STAGE_READ_OPERANDS,
    STAGE_EXECUTION_COMPLETE,
    STAGE_WRITE_RESULTS,
    STAGE_DONE,
} stage;


/*
 * Struct que contém as informações (status) de uma instrução.
 */
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


/*
 * Struct que contém as informações (status) do scoreboard.
 */
typedef struct scoreboard
{
    instruction_status *inst;
    reg_status *regs;
    uf_status *uf;

    size_t n_ufs;
    size_t n_registers;
    size_t inst_size;
    size_t inst_capacity;
} scoreboard;

scoreboard *scoreboard_init(int n_registers, config *cfg);
scoreboard *scoreboard_copy(scoreboard *sb);
void scoreboard_destroy(scoreboard *sb);

uint32_t scoreboard_add_instruction(scoreboard *sb, instruction_status is);
instruction_status *scoreboard_get_instruction(scoreboard *sb, int iid);
void scoreboard_expand_instructions(scoreboard *sb);
size_t scoreboard_n_instructions(scoreboard *sb);

#endif