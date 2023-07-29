#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitor_sb.h"
#include "instrucao.h"

int main(void)
{
    char *buf = arquivo_para_string("test/exemplo.sb");
    int l = strlen(buf);

    inst *instrucoes;
    config cfg;
    int n_instrucoes;

    le_sb(buf, (buf + l), &cfg, &instrucoes, &n_instrucoes);

    for (int i = 0; i < n_instrucoes; i++)
    {
        printf("%08x\t%d\n", instrucoes[i].instrucao, instrucoes[i].i.opcode);
    }

    free(instrucoes);
    free(buf);

    return 0;
}