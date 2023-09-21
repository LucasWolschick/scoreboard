#include "scoreboard.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

scoreboard *scoreboard_init(int n_registers, config *cfg)
{
    scoreboard *status = malloc(sizeof(scoreboard));
    status->n_registers = n_registers;

    status->regs = calloc(sizeof(reg_status), status->n_registers);
    for (int r = 0; r < status->n_registers; r++)
    {
        status->regs[r].uf = -1;
    }

    int n_ufs = cfg->n_uf_add + cfg->n_uf_int + cfg->n_uf_mul;
    status->n_ufs = n_ufs;
    status->uf = calloc(sizeof(uf_status), n_ufs);
    for (int u = 0; u < n_ufs; u++)
    {
        if (u < cfg->n_uf_add)
        {
            status->uf[u].type = FU_ADD;
            status->uf[u].n_type = u;
        }
        else if (u < cfg->n_uf_add + cfg->n_uf_int)
        {
            status->uf[u].type = FU_INT;
            status->uf[u].n_type = u - cfg->n_uf_add;
        }
        else
        {
            status->uf[u].type = FU_MUL;
            status->uf[u].n_type = u - cfg->n_uf_add - cfg->n_uf_int;
        }
        status->uf[u].busy = false;
        status->uf[u].op = 0;
        status->uf[u].fi = -1;
        status->uf[u].fj = -1;
        status->uf[u].fk = -1;
        status->uf[u].qj = -1;
        status->uf[u].qk = -1;
        status->uf[u].rj = false;
        status->uf[u].rk = false;
    }

    status->inst = calloc(sizeof(instruction_status), 2);
    status->inst_size = 0;
    status->inst_capacity = 2;

    return status;
}

scoreboard *scoreboard_copy(scoreboard *sb)
{
    scoreboard *copy = malloc(sizeof(scoreboard));

    copy->inst = malloc(sizeof(instruction_status) * sb->inst_capacity);
    copy->inst_size = sb->inst_size;
    copy->inst_capacity = sb->inst_capacity;
    memcpy(copy->inst, sb->inst, sizeof(instruction_status) * sb->inst_size);

    copy->regs = malloc(sizeof(reg_status) * sb->n_registers);
    copy->n_registers = sb->n_registers;
    memcpy(copy->regs, sb->regs, sizeof(reg_status) * sb->n_registers);

    copy->uf = malloc(sizeof(uf_status) * sb->n_ufs);
    copy->n_ufs = sb->n_ufs;
    memcpy(copy->uf, sb->uf, sizeof(uf_status) * sb->n_ufs);

    return copy;
}

void scoreboard_destroy(scoreboard *status)
{
    free(status->inst);
    free(status->regs);
    free(status->uf);
    free(status);
}

uint32_t scoreboard_add_instruction(scoreboard *sb, instruction_status is)
{
    if (sb->inst_size == sb->inst_capacity)
    {
        scoreboard_expand_instructions(sb);
    }

    sb->inst[sb->inst_size] = is;
    return sb->inst_size++;
}

instruction_status *scoreboard_get_instruction(scoreboard *sb, int iid)
{
    if (iid < 0 || iid >= sb->inst_size)
    {
        fprintf(stderr, "Tentativa de acesso aos dados da instrucaoo fora do intervalo\n");
    }
    return &sb->inst[iid];
}

void scoreboard_expand_instructions(scoreboard *sb)
{
    if (sb->inst_capacity < 2)
    {
        sb->inst_capacity = 2;
    }
    else
    {
        sb->inst_capacity *= 2;
    }
    sb->inst = realloc(sb->inst, sizeof(instruction_status) * sb->inst_capacity);
}

size_t scoreboard_n_instructions(scoreboard *sb)
{
    return sb->inst_size;
}