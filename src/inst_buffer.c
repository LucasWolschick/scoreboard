#include "inst_buffer.h"


/*
 * inst_buffer_init inicializa uma capacidade para o buffer de instruções.
 * Retorna um ponteiro para o buffer de instruções.
 */
inst_buffer *inst_buffer_init(size_t capacity)
{
    inst_buffer *buffer = malloc(sizeof(inst_buffer) + capacity * sizeof(uint32_t));
    buffer->capacity = capacity;
    buffer->head = 0;
    buffer->tail = 0;

    return buffer;
}

/*
 * inst_buffer_destroy recebe um ponteiro para um buffer de instruções e libera a memórias
 */
void inst_buffer_destroy(inst_buffer *buffer)
{
    free(buffer);
}

/*
 * inst_buffer_queue recebe um ponteiro para um buffer de instruções e uma instrução de 32 bits (sem sinal)
 * e coloca a instrução no buffer de instruções.
 */
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

/*
 * inst_buffer_peek recebe um ponteiro para um buffer de instruções e retorna
 * a instrução que está na cabeça do buffer de instruções (sem removê-la do buffer).
 */
uint32_t inst_buffer_peek(inst_buffer *buffer)
{
    if (inst_buffer_size(buffer) == 0)
    {
        fprintf(stderr, "Buffer de instrucao vazio\n");
        return -1;
    }

    return buffer->instructions[buffer->tail];
}


/*
 * inst_buffer_dequeue recebe um ponteiro para um buffer de instruções.
 * Remove e retorna a proxima instrução no buffer de instruções.
 */
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


/*
 * inst_buffer_size recebe um ponteiro para um buffer de instruções e retorna
 * o número de instruções no buffer de instruções.
 */
size_t inst_buffer_size(inst_buffer *buffer)
{
    return (buffer->head - buffer->tail + buffer->capacity) % buffer->capacity;
}

/*
 * inst_buffer_full recebe um ponteiro para um buffer de instruções e retorna
 * um bool se o buffer de instruções está cheio.
 */
bool inst_buffer_full(inst_buffer *buffer)
{
    return inst_buffer_size(buffer) == buffer->capacity;
}