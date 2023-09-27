#include "tables.h"

/* print_tables_instruction() recebe como parâmetro um scoreboard e imprime a
 * tabela de instruções com base no estado atual do scoreboard.
 */
void print_tables_instruction(scoreboard *sb)
{
    char ibuf[20] = {0};

    printf("*---------------------------------------------------------------------------------*\n");
    printf("| TABELA DE INSTRUCOES                                                            |\n");
    printf("*---------------------*----------*----------*----------*----------*---------------*\n");
    printf("|      Instrucao      |  Busca   | Emissao  |  L. Ops  | Execucao | E. Resultados |\n");
    printf("*---------------------*----------*----------*----------*----------*---------------*\n");
    for (int i = 0; i < sb->inst_size; i++)
    {
        int wrote = snprint_instruction(sb->inst[i].instruction, ibuf, 20);
        if (wrote > 19)
        {
            for (int i = 0; i < 3; i++)
            {
                ibuf[wrote - i - 1] = '.';
            }
        }
        printf("| %19s |", ibuf);
        for (stage s = STAGE_FETCH; s <= STAGE_WRITE_RESULTS; s++)
        {
            int offset = s == STAGE_WRITE_RESULTS ? 13 : 8;
            if (sb->inst[i].when[s] == -1)
            {
                printf(" %*s |", offset, " ");
            }
            else
            {
                printf(" %*d |", offset, sb->inst[i].when[s]);
            }
        }
        printf("\n");
    }
    printf("*---------------------*----------*----------*----------*----------*---------------*\n");
    printf("\n");
}

/* print_tables_ufs() recebe como parâmetro um scoreboard e imprime a tabela
 * de unidades funcionais com base no estado atual do scoreboard.
 */
void print_tables_ufs(scoreboard *sb, register_bank *regs)
{
    const char values[2] = {'F', 'V'};
    printf("*------------------------------------------------------------------------------------------------------*\n");
    printf("| TABELA DAS UNIDADES FUNCIONAIS                                                                       |\n");
    printf("*---------*------*------*------*------------------*------------------*---------*---------*------*------*\n");
    printf("|    UF   | busy |  op  |  Fi  |        Fj        |        Fk        |    Qj   |    Qk   |  Rj  |  Rk  |\n");
    printf("*---------*------*------*------*------------------*------------------*---------*---------*------*------*\n");

    for (int i = 0; i < sb->n_ufs; i++)
    {
        // uf name
        switch (sb->uf[i].type)
        {
        case FU_ADD:
            printf("|  Add%-3d |", sb->uf[i].n_type + 1);
            break;
        case FU_MUL:
            printf("|  Mul%-3d |", sb->uf[i].n_type + 1);
            break;
        case FU_INT:
            printf("|  Int%-3d |", sb->uf[i].n_type + 1);
        }

        // busy
        bool busy = sb->uf[i].busy;
        printf("   %c  |", values[sb->uf[i].busy]);

        // op
        printf("%5s |", !busy ? "" : string_from_opcode(sb->uf[i].op));

        // fi
        if (!busy || sb->uf[i].fi == -1)
        {
            printf("      |");
        }
        else
        {
            switch (sb->uf[i].op)
            {
            case OP_BEQ:
            case OP_BNE:
            case OP_BLT:
            case OP_BGT:
            case OP_J:
                printf("      |");
                break;
            default:
                printf("  R%-2d |", sb->uf[i].fi);
            }
        }

        // fj, fk (must be of the form Rn = (register value))
        if (!busy || sb->uf[i].fj == -1)
        {
            printf("                  |");
        }
        else
        {
            printf(" R%-2d = %-10d |", sb->uf[i].fj, register_read(regs, sb->uf[i].fj));
        }

        if (!busy || sb->uf[i].fk == -1)
        {
            printf("                  |");
        }
        else
        {
            switch (sb->uf[i].op)
            {
            case OP_ADDI:
            case OP_SUBI:
            case OP_LW:
            case OP_SW:
                printf(" %-16d |", sb->uf[i].fk);
                break;
            case OP_J:
                printf(" %-16d |", sb->uf[i].fk);
                break;
            default:
                printf(" R%-2d = %-10d |", sb->uf[i].fk, register_read(regs, sb->uf[i].fk));
            }
        }

        // qj, qk (functional unit names just like as in UF)
        if (!busy || sb->uf[i].qj == -1)
        {
            printf("         |");
        }
        else
        {
            switch (sb->uf[sb->uf[i].qj].type)
            {
            case FU_ADD:
                printf("  Add%-3d |", sb->uf[sb->uf[i].qj].n_type + 1);
                break;
            case FU_MUL:
                printf("  Mul%-3d |", sb->uf[sb->uf[i].qj].n_type + 1);
                break;
            case FU_INT:
                printf("  Int%-3d |", sb->uf[sb->uf[i].qj].n_type + 1);
            }
        }
        if (!busy || sb->uf[i].qk == -1)
        {
            printf("         |");
        }
        else
        {
            switch (sb->uf[sb->uf[i].qk].type)
            {
            case FU_ADD:
                printf("  Add%-3d |", sb->uf[sb->uf[i].qk].n_type + 1);
                break;
            case FU_MUL:
                printf("  Mul%-3d |", sb->uf[sb->uf[i].qk].n_type + 1);
                break;
            case FU_INT:
                printf("  Int%-3d |", sb->uf[sb->uf[i].qk].n_type + 1);
            }
        }

        // rj, rk
        printf("   %c  |", busy ? values[sb->uf[i].rj] : ' ');
        printf("   %c  |", busy ? values[sb->uf[i].rk] : ' ');
        printf("\n");
    }
    printf("*---------*------*------*------*------------------*------------------*---------*---------*------*------*\n");
    printf("\n");
}

/* print_tables_regs() recebe como parâmetro um scoreboard e imprime a tabela
 * de registradores, mais especificamente qual a unidade funcional que gerou o
 * resultado armazenado em cada registrador, com base no estado atual do scoreboard.
 */
void print_tables_regs(scoreboard *sb)
{
    printf("*-------------------------*\n");
    printf("| TABELA DE REGISTRADORES |\n");
    printf("*------*------------------*\n");
    printf("|      |        UF        |\n");
    printf("*------*------------------*\n");
    for (int i = 0; i < N_REGISTERS; i++)
    {
        printf("|  R%-2d |", i);
        if (sb->regs[i].uf == -1)
        {
            printf(" %16s |", "");
            printf("\n");
        }
        else
        {
            switch (sb->uf[sb->regs[i].uf].type)
            {
            case FU_ADD:
                printf(" Add%-3d           |", sb->uf[sb->regs[i].uf].n_type + 1);
                break;
            case FU_MUL:
                printf(" Mul%-3d           |", sb->uf[sb->regs[i].uf].n_type + 1);
                break;
            case FU_INT:
                printf(" Int%-3d           |", sb->uf[sb->regs[i].uf].n_type + 1);
                break;
            }
            printf("\n");
        }
    }
    printf("*------*------------------*\n");
    printf("\n");
}

/* print_tables_regs_f() recebe como parâmetro um scoreboard e imprime a tabela
 * de registradores, mais especificamente qual o valor concreto armazenado em cada
 * registrador, com base no estado atual do scoreboard.
 */
void print_tables_regs_f(scoreboard *sb, register_bank *r)
{
    printf("*-------------------------*\n");
    printf("| TABELA DE REGISTRADORES |\n");
    printf("*------*------------------*\n");
    printf("|      |        Valor     |\n");
    printf("*------*------------------*\n");
    for (int i = 0; i < N_REGISTERS; i++)
    {
        printf("|  R%-2d |", i);
        printf(" %16d |", register_read(r, i));
        printf("\n");
    }
    printf("*------*------------------*\n");
    printf("\n");
}

/* print_memory() recebe como parâmetro uma memória e imprime o estado atual
 * da memória, o que inclui o endereço da memória e o valor armazenado.
 */
void print_memory(memory *m)
{
    printf("*------------------------------------------------------------*\n");
    printf("| ESTADO DA MEMORIA                                          |\n");
    printf("*----------*-------------------------------------------------*\n");
    printf("| Endereco | Valores                                         |\n");
    printf("*----------*-------------------------------------------------*\n");
    for (int i = 0; i < m->length; i++)
    {
        if (i % 16 == 0)
        {
            printf("| %08x | ", i);
        }
        printf("%02x ", memory_read(m, i));
        if (i % 16 == 15)
        {
            printf("|\n");
        }
    }
    if (m->length % 16 != 0)
    {
        for (int i = 0; i < 16 - (m->length % 16); i++)
        {
            printf("   ");
        }
        printf("|\n");
    }
    printf("*----------*-------------------------------------------------*\n");
    printf("\n");
}

/* print_tables() recebe como parâmetro um scoreboard e um banco de registradores
 * e imprime as tabelas que demonstram o estado do scoreboard em um ciclo de clock
 * específico (tabela de instruções, tabela de unidades funcionais e tabela de
 * registradores) com base no estado atual do scoreboard.
 */
void print_tables(scoreboard *sb, register_bank *regs)
{
    print_tables_instruction(sb);
    print_tables_ufs(sb, regs);
    print_tables_regs(sb);
}
