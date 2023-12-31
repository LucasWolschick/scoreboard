#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "consts.h"
#include "parser.h"
#include "cpu.h"
#include "bus.h"
#include "memory.h"
#include "register.h"
#include "func_unit.h"
#include "tables.h"

/**
 * SIMULADOR DE SCOREBOARDING
 * Universidade Estadual de Maringá
 * Arquitetura e Organização de Computadores II
 *
 * Para instruções de uso e compilação, vide README.md
 *
 * Autores:
 * - Guilherme Frare Clemente (ra124349@uem.br)
 * - Lucas Wolschick (ra123658@uem.br)
 * - Marcos Vinícius de Oliveira (ra124408@uem.br)
 */

/*
 * file_to_string recebe o caminho de um arquivo e retorna uma string com o conteúdo do arquivo.
 */
char *file_to_string(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        perror("Erro abrindo arquivo");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(sizeof(char) * (len + 1));
    fread(buf, sizeof(char), len, f);
    buf[len] = '\0';

    fclose(f);

    return buf;
}

/*
 * main recebe os argumentos da linha de comando e executa o simulador.
 * Uso: ./simulador -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]
 */
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Uso: %s -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]\n", argv[0]);
        exit(1);
    }

    char *programFileName = NULL;
    int memorySize = 0;
    char *outputFileName = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if (argc < i + 2)
            {
                fprintf(stderr, "Uso: %s -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]\n", argv[0]);
                exit(1);
            }
            programFileName = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            if (argc < i + 2)
            {
                fprintf(stderr, "Uso: %s -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]\n", argv[0]);
                exit(1);
            }
            memorySize = strtol(argv[i + 1], NULL, 10);
            if (memorySize < MIN_TAM_MEMO)
            {
                fprintf(stderr, "Erro: a memoria deve ter no minimo 100 bytes de comprimento.\n");
                exit(1);
            }
            i++;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            if (argc < i + 2)
            {
                fprintf(stderr, "Uso: %s -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]\n", argv[0]);
                exit(1);
            }
            outputFileName = argv[i + 1];
            i++;
        }
    }

    if (programFileName == NULL)
    {
        fprintf(stderr, "Uso: %s -p <nome do programa> -m <tamanho da memória> [-o <nome do arquivo de saida>]\n", argv[0]);
        exit(1);
    }

    if (outputFileName)
    {
        if (!freopen(outputFileName, "w", stdout))
        {
            perror("Erro abrindo arquivo de saida");
            exit(1);
        }
    }

    bool interactive = !outputFileName;

    char *b = file_to_string(programFileName);
    vector data, instrucoes;
    config cfg;
    parse(b, &data, &instrucoes, &cfg);

    if (vector_len(&data) * sizeof(vector_get(&data, 0, 0)) > 100)
    {
        fprintf(stderr, "Erro: a secao de dados deve ter no maximo 100 bytes de comprimento.\n");
        exit(1);
    }

    if (vector_len(&instrucoes) * sizeof(vector_get(&instrucoes, 0, 0)) > memorySize - 100)
    {
        fprintf(stderr, "Erro: a secao de instrucoes deve ter no maximo %d bytes de comprimento.\n", memorySize - 100);
        exit(1);
    }

    // inicializa registradores
    // inicializa memória
    memory *mem = memory_init(memorySize);

    // obs: nosso computador é little endian
    // escreve .data nos primeiros 100 bytes
    for (int i = 0; i < vector_len(&data); i++)
    {
        uint32_t datum;
        vector_get(&data, i, &datum);

        for (int j = 0; j < sizeof(datum); j++)
        {
            uint8_t byte = (datum >> (8 * (sizeof(datum) - j - 1))) & 0xFF;
            memory_write(mem, i * sizeof(datum) + j, byte);
        }
    }

    // escreve .text nos bytes 100 em diante
    for (int i = 0; i < vector_len(&instrucoes); i++)
    {
        uint32_t instrucao;
        vector_get(&instrucoes, i, &instrucao);

        for (int j = 0; j < sizeof(instrucao); j++)
        {
            uint8_t byte = (instrucao >> (8 * (sizeof(instrucao) - j - 1))) & 0xFF;
            memory_write(mem, 100 + i * sizeof(instrucao) + j, byte);
        }
    }

    // inicializa unidades funcionais
    int n_ufs = cfg.n_uf_add + cfg.n_uf_mul + cfg.n_uf_int;
    uf *ufs = calloc(sizeof(uf), n_ufs);

    // inicializa cpu
    cpu *c = cpu_init(NULL, NULL, cfg, memorySize);

    // inicializa scoreboard
    scoreboard *sb = scoreboard_init(N_REGISTERS, &cfg);

    // inicializa banco de registradores
    register_bank *regs = register_init();
    register_write_pc(regs, 100);

    // inicializa barramento
    bus *barramento = bus_init(c, regs, ufs, sb);
    c->bus = barramento;

    // inicializa barramento do sistema
    sys_bus *sys_barramento = sys_bus_init(mem, c);
    c->sys_bus = sys_barramento;

    // põe pra rodar
    printf("** ESTADO INICIAL DA MEMORIA:\n");
    print_memory(c->sys_bus->memory);
    printf("** CLOCK = %d\n", c->ck);
    while (pipeline(c))
    {
        print_tables(c->bus->board, c->bus->registers);
        if (interactive)
            getchar();
        printf("** CLOCK = %d\n", c->ck);
    }
    print_tables(c->bus->board, c->bus->registers);
    printf("** FIM (levou %d ciclos)\n", c->ck);
    print_tables_regs_f(c->bus->board, c->bus->registers);
    print_memory(c->sys_bus->memory);

    // destroi as coisas
    sys_bus_destroy(sys_barramento);
    scoreboard_destroy(c->bus->board);
    bus_destroy(barramento);
    register_destroy(regs);
    cpu_destroy(c);
    free(ufs);
    memory_destroy(mem);
    vector_destroy(&data);
    vector_destroy(&instrucoes);
    free(b);

    return 0;
}