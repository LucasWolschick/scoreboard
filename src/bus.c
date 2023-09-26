#include "bus.h"

/*
 * bus_init inicializa um struct bus que representa o barramento e 
 * retorna um ponteiro para ele que deve ser destruído com o bus_destroy.
 */ 
bus *bus_init(cpu *c, register_bank *r, uf *f, scoreboard *sb)
{
    bus *b = malloc(sizeof *b);
    b->cpu = c;
    b->registers = r;
    b->func_units = f;
    b->board = sb;
    return b;
}


/*
 * bus_destroy recebe um struct bus que representa o barramento em si,
 * liberando a memória do mesmo.
 */
void bus_destroy(bus *b)
{
    free(b);
}

/*
 * bus_write_pc recebe um struct bus, que representa o barramento, e um número inteiro de 32 bits (PC) sem sinal,
 * escrevendo os valores do contador de programa no registrador.
 */
void bus_write_pc(bus *b, uint32_t pc)
{
    register_write_pc(b->registers, pc);
}


/*
 * bus_read_pc recebe um struct bus que representa o barramento,
 * retornando a leitura dos valores do contador de programa (PC) no registrador,
 * utilizando um inteiro de 32 bits (sem sinal) como retorno.
 */
uint32_t bus_read_pc(bus *b)
{
    return register_read_pc(b->registers);
}


/*
 * bus_read_ir recebe um struct bus que representa o barramento,
 * e retorna a leitura dos valores do registrador de instruções (IR) como um inteiro de 32 bits (sem sinal).
 */
uint32_t bus_read_ir(bus *b)
{
    return register_read_ir(b->registers);
}


/*
 * bus_write_ir recebe um struct bus, que representa o barramento, e
 * um número inteiro de 32 bits (instrução) sem sinal , retornando a escrita dos valores do registrador
 * de instruções (IR). 
 */
void bus_write_ir(bus *b, uint32_t instruction)
{
    register_write_ir(b->registers, instruction);
}


/*
 * bus_load_pc_ir recebe um struct bus, que representa o barramento, e um struct
 * sys_bus que representa o barramento de sistema, dessa forma irá ler o contador de programa (PC),
 * a partir disso, ler a instrução e por último escrever a instrução no registrador de instrução (IR).
 */
void bus_load_pc_ir(bus *b, sys_bus *sb)
{
    uint32_t pc = bus_read_pc(b);
    uint32_t instruction = sys_bus_read_memory_word(sb, pc);
    bus_write_ir(b, instruction);
}


/*
 * bus_read_reg recebe um struct bus, que representa o barramento, e um número inteiro
 * de 8 bits (número de registradores) sem sinal, retornando a leitura dos valores dentro do banco de registadores.
 */
uint32_t bus_read_reg(bus *b, uint8_t reg)
{
    return register_read(b->registers, reg);
}


/*
 * bus_write_reg recebe um struct bus, que representa o barramento, e dois números inteiros,
 * um de 32 bits (instrução) e 8 bits (número de registradores), ambos sem sinal, escrevendo o value dentro de um registrador especifico do banco
 * de registradores.
 */
void bus_write_reg(bus *b, uint8_t reg, uint32_t value)
{
    register_write(b->registers, reg, value);
}

/*
 * bus_func_unit_load_instruction recebe um struct bus, que representa o barramento, juntamente com
 * dois números inteiros, ambos de 32 bis (sem sinal), que representa a unidade, 
 * resetando a unidade funcional e carregando a instrução.
 */
void bus_func_unit_load_instruction(bus *b, uint32_t unit, uint32_t instruction)
{
    uf_load_instruction(&b->func_units[unit], instruction);
}

/*
 * bus_func_unit_load_ops recebe um struct bus, que representa o barrameto, struct sys_bus, que representa
 * o barramento do sistema, e um número inteiro de 32 bits (sem sinal) que representa a unidade, onde irá ocorrer
 * a leitura de operandos da unidade funcional fornecida. 
 */
void bus_func_unit_load_ops(bus *b, uint32_t unit, sys_bus *sb)
{
    uf_load_ops(&b->func_units[unit], b, sb);
}


/*
 * bus_func_unit_write_res recebe um struct bus, que represeta o barramento, struct sys_bus, que representa
 * o barramento do sistema, e um número inteiro de 32 bits (sem sinal) que representa a unidade, onde irá ocorrer
 * a escrita de resultados da unidade funcional.
 */
void bus_func_unit_write_res(bus *b, uint32_t unit, sys_bus *sb)
{
    uf_write_res(&b->func_units[unit], b, sb);
}


/*
 * bus_sb_get_register_status recebe um struct bus, que representa o barramento, e um número inteiro de 8 bits
 * (sem sinal) que representa o número de registadores, retornando o status desse registrador reg.
 */
reg_status *bus_sb_get_register_status(bus *b, uint8_t reg)
{
    return &b->board->regs[reg];
}


/*
 * bus_sb_get_func_unit_status recebe um struct bus, que representa o barramento, e um número inteiro de 32 bits
 * (sem sinal) que representa a unidade, retornando o status da unidade funcional fornecida.
 */
uf_status *bus_sb_get_func_unit_status(bus *b, uint32_t unit)
{
    return &b->board->uf[unit];
}

/*
 * bus_sb_add_instruction recebe um struct bus, que representa o barramento, e  os status da instrução fornecida.
 * Adicionando a intrução na lista do barramento.
 */
uint32_t bus_sb_add_instruction(bus *b, instruction_status is)
{
    return scoreboard_add_instruction(b->board, is);
}

/*
 * bus_sb_get_instruction recebe um struct bus, que representa o baramento, e um número inteiro.
 * Retornando a instrução em si.
 */
instruction_status *bus_sb_get_instruction(bus *b, int iid)
{
    return scoreboard_get_instruction(b->board, iid);
}

/*
 * bus_sb_n_instructions recebe um struct bus, que representa o barramento, e retorna o número de instruções
 * presente dentro do histórico do Scoreboard.
 */
size_t bus_sb_n_instructions(bus *b)
{
    return scoreboard_n_instructions(b->board);
}