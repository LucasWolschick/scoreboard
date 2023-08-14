#include "memory.h"

memory *memory_init(uint32_t length)
{
    memory *m = malloc(sizeof *m);
    m->length = length;
    m->data = calloc(length, sizeof m->data[0]);
    return m;
}

void memory_destroy(memory *m)
{
    free(m->data);
    free(m);
}

uint8_t memory_read(memory *m, uint32_t address)
{
    if (address >= m->length)
    {
        fprintf(stderr, "Leitura fora do espaco de memoria; abortando\n");
        exit(1);
    }
    return m->data[address];
}
void memory_write(memory *m, uint32_t address, uint8_t value)
{
    if (address >= m->length)
    {
        fprintf(stderr, "Escrita fora do espaco de memoria; abortando\n");
        exit(1);
    }
    m->data[address] = value;
}