#include "sys_bus.h"
#include "memory.h"

/* sys_bus_init() inicializa o barramento de sistema, tendo como parâmetros a memória
 * e a CPU. Retorna um ponteiro para o barramento de sistema inicializado.
 */
sys_bus *sys_bus_init(memory *m, cpu *c)
{
    sys_bus *b = malloc(sizeof(sys_bus));
    b->memory = m;
    b->cpu = c;
    return b;
}

/* sys_bus_destroy() libera a memória alocada para o barramento de sistema
 * passado como parâmetro.
 */
void sys_bus_destroy(sys_bus *b)
{
    free(b);
}

/* sys_bus_read_register() lê o valor armazenado especificado em um
 * barramento do sistema no endereço address, também passado por parâmetro. Tem como
 * retorno o valor armazenado.
 */
uint8_t sys_bus_read_memory(sys_bus *b, uint32_t address)
{
    return memory_read(b->memory, address);
}

/* sys_bus_write_register() escreve o valor passado como parâmetro
 * um barramento do sistema no endereço address, também passado
 * por parâmetro.
 */
void sys_bus_write_memory(sys_bus *b, uint32_t address, uint8_t value)
{
    memory_write(b->memory, address, value);
}

/* sys_bus_read_register_word() lê o valor armazenado barramento do sistema no
 * endereço address, porém diferente da função sys_bus_read_memory(), esta função
 * lê uma palavra (4 bytes = 32 bits) a partir do endereço address,
 * também passado por parâmetro. Tem como retorno o valor armazenado.
 */
uint32_t sys_bus_read_memory_word(sys_bus *b, uint32_t address)
{
    return memory_read_word(b->memory, address);
}

/* sys_bus_write_register_word() escreve o valor passado como parâmetro
 * um barramento do sistema no endereço address, também passado
 * por parâmetro. Diferente da função sys_bus_write_memory(), esta função
 * escreve uma palavra (4 bytes = 32 bits) a partir do endereço address.
 */
void sys_bus_write_memory_word(sys_bus *b, uint32_t address, uint32_t value)
{
    memory_write_word(b->memory, address, value);
}