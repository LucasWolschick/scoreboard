#include "inst_buffer.h"

inst_buffer *inst_buffer_init(size_t capacity)
{
    // inst_buffer has a flexible array member instructions
    // so we need to allocate enough memory for the struct
    // and the array
    inst_buffer *buffer = malloc(sizeof(inst_buffer) + capacity * sizeof(uint32_t));
    buffer->capacity = capacity;
    buffer->head = 0;
    buffer->tail = 0;

    return buffer;
}

void inst_buffer_destroy(inst_buffer *buffer)
{
    free(buffer);
}

void inst_buffer_queue(inst_buffer *buffer, uint32_t instruction)
{
    if (inst_buffer_full(buffer))
    {
        fprintf(stderr, "Buffer de instrucao cheio\n");
        return;
    }

    buffer->instructions[buffer->head] = instruction;
    buffer->head = (buffer->head + 1) % buffer->capacity;
}

uint32_t inst_buffer_peek(inst_buffer *buffer)
{
    if (inst_buffer_size(buffer) == 0)
    {
        fprintf(stderr, "Buffer de instrucao vazio\n");
        return -1;
    }

    return buffer->instructions[buffer->tail];
}

uint32_t inst_buffer_dequeue(inst_buffer *buffer)
{
    if (inst_buffer_size(buffer) == 0)
    {
        fprintf(stderr, "Buffer de instrucao vazio\n");
        return -1;
    }

    uint32_t instruction = buffer->instructions[buffer->tail];
    buffer->tail = (buffer->tail + 1) % buffer->capacity;
    return instruction;
}

size_t inst_buffer_size(inst_buffer *buffer)
{
    return (buffer->head - buffer->tail + buffer->capacity) % buffer->capacity;
}

bool inst_buffer_full(inst_buffer *buffer)
{
    return inst_buffer_size(buffer) == buffer->capacity;
}