#ifndef CONFIG_H
#define CONFIG_H

#include "opcode.h"

typedef struct config
{
    // num de unidades funcionais
    int n_uf_add;
    int n_uf_mul;
    int n_uf_int;

    // ciclos por instrucao
    int ck_instruction[OP_EXIT + 1];

    // unidades funcionai por grupo
    int ufs_per_group;
} config;

#endif