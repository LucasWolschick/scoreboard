#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "parser.h"

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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <caminho>\n", argv[0]);
        exit(1);
    }

    char *b = file_to_string(argv[1]);
    int n_instrucoes;
    vector data, instrucoes;
    parse(b, &data, &instrucoes);

    puts("Dados:");
    for (int i = 0; i < vector_len(&data); i++)
    {
        int e;
        vector_get(&data, i, &e);
        printf("%5d: %08x\n", i, e);
    }

    puts("Instrucoes:");
    for (int i = 0; i < vector_len(&instrucoes); i++)
    {
        int e;
        vector_get(&instrucoes, i, &e);
        printf("%5d: %08x\n", i + 100, e);
    }

    vector_destroy(&data);
    vector_destroy(&instrucoes);
    free(b);

    return 0;
}