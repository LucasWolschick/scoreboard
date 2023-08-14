#include <stdio.h>
#define MIN_TAM_MEMO 100

int main(char argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Uso: %s -p nome do programa> -m <tamanho da memória> [-o <nome do arquivo>]\n", argv[0]);
        exit(1);
    }

    char *programFileName = NULL;
    int memorySize = 0;
    char *outputFileName = NULL;

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-p") == 0)
        {
            programFileName = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            memorySize = atoi(argv[i + 1]);
            if(memorySize < MIN_TAM_MEMO) exit(1);
            i++;
        }
        else if(strcmp(argv[i], "-o") == 0)
        {
            outputFileName = argv[i + 1];
            i++;
        }
    }

    if(programFileName == NULL || memorySize < MIN_TAM_MEMO)
    {
        fprintf(stderr, "A memoria estourou o limite!");
        exit(1);
    }

    //Substituir oque esta comentado pelas linhas 79 - 82
        // char *b = file_to_string(programFileName);
        // int n_instrucoes;
        // vector data, instrucoes;
        // parse(b, &data, &instrucoes);


}