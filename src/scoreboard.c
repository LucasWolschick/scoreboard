#include "scoreboard.h"

#include <stdlib.h>
#include <stdio.h>

scoreboard *scoreboard_init(int n_registers, config *cfg)
{
    scoreboard *status = malloc(sizeof(scoreboard));

    // for (int i = 0; i < n_instructions; i++)
    // {
    //     status->inst[i].st = STAGE_FETCH;
    //     status->inst[i].uf = -1;
    //     for (stage s = STAGE_FETCH; s <= STAGE_DONE; s++)
    //     {
    //         status->inst[i].when[s] = -1;
    //     }
    // }

    status->regs = calloc(sizeof(reg_status), n_registers);
    for (int r = 0; r < n_registers; r++)
    {
        status->regs[r].uf = -1;
    }

    int n_ufs = cfg->n_uf_add + cfg->n_uf_int + cfg->n_uf_mul;
    status->uf = calloc(sizeof(uf_status), n_ufs);
    for (int u = 0; u < n_ufs; u++)
    {
        if (u < cfg->n_uf_add)
        {
            status->uf[u].type = FU_ADD;
        }
        else if (u < cfg->n_uf_add + cfg->n_uf_int)
        {
            status->uf[u].type = FU_INT;
        }
        else
        {
            status->uf[u].type = FU_MUL;
        }
    }

    status->inst = calloc(sizeof(instruction_status), 2);
    status->inst_size = 0;
    status->inst_capacity = 2;

    return status;
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