#include "scoreboard.h"

#include <stdlib.h>

scoreboard *scoreboard_init(int n_registers, config *cfg)
{
    scoreboard *status = malloc(sizeof(scoreboard));

    // TODO: fazer vetor dinâmico de instruções
    int n_instructions = 100000;
    status->inst = calloc(sizeof(instruction_status), n_instructions);
    for (int i = 0; i < n_instructions; i++)
    {
        status->inst[i].st = STAGE_FETCH;
        status->inst[i].uf = -1;
        for (stage s = STAGE_FETCH; s <= STAGE_DONE; s++)
        {
            status->inst[i].when[s] = -1;
        }
    }

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

    return status;
}

void scoreboard_destroy(scoreboard *status)
{
    free(status->inst);
    free(status->regs);
    free(status->uf);
    free(status);
}