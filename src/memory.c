#include "memory.h"


/*
 * memory_init inicializa uma memória com um tamanho especificado.
 * Retorna um ponteiro para a memória.
 */
memory *memory_init(uint32_t length)
{
    memory *m = malloc(sizeof *m);
    m->length = length;
    m->data = calloc(length, sizeof m->data[0]);
    return m;
}

/*
 * memory_destroy recebe um ponteiro para uma memória e libera a memória.
 */
void memory_destroy(memory *m)
{
    free(m->data);
    free(m);
}


/*
 * memory_read recebe um ponteiro para uma memória e um endereço de memória de 32 bits (sem sinal).
 * Retorna o byte que está no endereço de memória especificado.
 */
uint8_t memory_read(memory *m, uint32_t address)
{
    if (address >= m->length)
    {
        fprintf(stderr, "Leitura fora do espaco de memoria; abortando\n");
        exit(1);
    }
    return m->data[address];
}

/*
 * memory_write recebe um ponteiro para uma memória, um endereço de memória de 32 bits (sem sinal) e
 * um value de 8 bits (sem sinal). Escreve o value no endereço de memória especificado.
 */
void memory_write(memory *m, uint32_t address, uint8_t value)
{
    if (address >= m->length)
    {
        fprintf(stderr, "Escrita fora do espaco de memoria; abortando\n");
        exit(1);
    }
    m->data[address] = value;
}

/*
 * memory_read_word recebe um ponteiro para uma memória e um endereço de memória de 32 bits (sem sinal).
 * Retorna a palavra de 32 bits que está no endereço de memória especificado.
 */
uint32_t memory_read_word(memory *m, uint32_t address)
{
    if (address >= m->length - 3)
    {
        fprintf(stderr, "Leitura fora do espaco de memoria; abortando\n");
        exit(1);
    }
    return (m->data[address] << 24) | (m->data[address + 1] << 16) | (m->data[address + 2] << 8) | m->data[address + 3];
}


/*
 * memory_write_word recebe um ponteiro para uma memória, um endereço de memória de 32 bits (sem sinal) e
 * um value de 32 bits (sem sinal). Escreve a palavra de 32 bits no endereço de memória especificado.
 */
void memory_write_word(memory *m, uint32_t address, uint32_t value)
{
    if (address >= m->length - 3)
    {
        fprintf(stderr, "Escrita fora do espaco de memoria; abortando\n");
        exit(1);
    }
    m->data[address] = (value >> 24) & 0xFF;
    m->data[address + 1] = (value >> 16) & 0xFF;
    m->data[address + 2] = (value >> 8) & 0xFF;
    m->data[address + 3] = value & 0xFF;
}