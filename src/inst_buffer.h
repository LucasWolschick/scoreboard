#ifndef INST_BUFFER_H
#define INST_BUFFER_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct inst_buffer
{
    size_t capacity;
    size_t head;
    size_t tail;
    uint32_t instructions[];
} inst_buffer;

inst_buffer *inst_buffer_init(size_t capacity);
void inst_buffer_destroy(inst_buffer *buffer);

void inst_buffer_queue(inst_buffer *buffer, uint32_t instruction);
uint32_t inst_buffer_peek(inst_buffer *buffer);
uint32_t inst_buffer_dequeue(inst_buffer *buffer);

size_t inst_buffer_size(inst_buffer *buffer);
bool inst_buffer_full(inst_buffer *buffer);

#endif