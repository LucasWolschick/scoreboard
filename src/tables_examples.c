/*
o inst é um vetor de informações por instrução
tem inst_size instruções nele
daí, dentro de cada instrução
tem os dados lá do scoreboard tal
tem um vetor em cada instrução, when, que relaciona o estágio do pipeline com o ciclo de clock que o estágio foi finalizado
tem como vc saber qual é o próximo estágio que ele vai executar pelo stage
daí lá guarda também a instrução codificada em um int de 32 bits sem sinal
vc pode ver o código print_instruction no cpu.c pra ver como extrair os campos daquele número
ok, o vetor uf tem n_ufs unidades funcionais e cada struct representa uma unidade funcional
tem os campos lá, eles são exatamente o que a gente viu em sala
guarda o opcode que tá executando também
por fim, tem o vetor regs
que é um mapeamento número do registrador -> uf que vai escrever, que nem a gente viu na sala
o ciclo de clock atual é guardado no struct cpu
acho isso é td pra printar as tabelas
*/
#include "tables_examples.h"

void print_tables_instruction(scoreboard *sb)
{

    printf("Instruction Table\n");
    printf("|  Fetch |  Issue  |  Read  |  Exec  |  Write  |\n");
    for (int i = 0; i < sb->inst_size; i++)
    {
        printf("|%8d|%9d|%8d|%8d|%9d|\n", sb->inst[i].when[STAGE_FETCH], sb->inst[i].when[STAGE_ISSUE], sb->inst[i].when[STAGE_READ_OPERANDS], sb->inst[i].when[STAGE_EXECUTION_COMPLETE], sb->inst[i].when[STAGE_WRITE_RESULTS]);
    }
    printf("\n");
}

void print_tables_ufs(scoreboard *sb)
{

    printf("UFs Table\n");
    printf("|  Busy |  Op  |  Fi  |  Fj  |  Fk  |  Qj  |  Qk  |  Rj  |  Rk  |\n");
    for (int i = 0; i < sb->n_ufs; i++)
    {

        printf("|%7d|%6d|%6d|%6d|%6d|%6d|%6d|%6d|%6d|\n", sb->uf[i].busy, sb->uf[i].op, sb->uf[i].fi, sb->uf[i].fj, sb->uf[i].fk, sb->uf[i].qj, sb->uf[i].qk, sb->uf[i].rj, sb->uf[i].rk);
    }
    printf("\n");
}

void print_tables_regs(scoreboard *sb)
{
    printf("Regs Tables\n");
    printf("|  UF  |\n");
    printf("|");
    for (int i = 0; i < sb->n_registers; i++)
    {
        printf("%4d|", sb->regs[i].uf);
    }

    printf("\n");
}

void print_tables(scoreboard *sb)
{
    print_tables_instruction(sb);
    print_tables_ufs(sb);
    print_tables_regs(sb);
}
