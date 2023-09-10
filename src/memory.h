#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct memory
{
    uint32_t length;
    uint8_t *data;
} memory;

memory *memory_init(uint32_t length);
void memory_destroy(memory *m);
uint8_t memory_read(memory *m, uint32_t address);
void memory_write(memory *m, uint32_t address, uint8_t value);
uint32_t memory_read_word(memory *m, uint32_t address);
void memory_write_word(memory *m, uint32_t address, uint32_t value);

#endif