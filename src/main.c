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
        printf("%d %d %d %d\n", instrucoes[i].opcode, instrucoes[i].op1, instrucoes[i].op2, instrucoes[i].op3);
    }

    free(instrucoes);
    free(buf);

    return 0;
}