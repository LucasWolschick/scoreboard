#include "sys_bus.h"
#include "memory.h"

sys_bus *sys_bus_init(memory *m, cpu *c)
{
    sys_bus *b = malloc(sizeof(sys_bus));
    b->memory = m;
    b->cpu = c;
    return b;
}

void sys_bus_destroy(sys_bus *b)
{
    free(b);
}

uint32_t sys_bus_read_memory(sys_bus *b, uint32_t address)
{
    return memory_read(b->memory, address);
}

void sys_bus_write_memory(sys_bus *b, uint32_t address, uint32_t value)
{
    memory_write(b->memory, address, value);
}