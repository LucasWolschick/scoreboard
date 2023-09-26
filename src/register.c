#include "register.h"

/* register_init() inicializa o banco de registradores, definindo o Program Counter
 * (PC) e o Instruction Register (IR) como 0 e os registradores de uso geral como 0.
 * retorno um ponteiro para o banco de registradores inicializado.
 */
register_bank *register_init(void)
{
    register_bank *r = malloc(sizeof(register_bank));
    r->pc = 0;
    r->ir = 0;
    for (int i = 0; i < N_REGISTERS; i++)
        r->regs[i] = 0;
    return r;
}

/* register_destroy() libera a memória alocada para o banco de registradores
 * passado como parâmetro.
 */
void register_destroy(register_bank *r)
{
    free(r);
}

/* register_write() escreve o valor passado como parâmetro no registrador
 * especificado em um certo banco de registradores, também passados por parâmetro.
 * Se o registrador for o 0 (registrador r0), o valor não é escrito pois ele tem
 * armazenado o valor 0 como constante e não pode ser alterado.
 */
void register_write(register_bank *r, uint8_t reg, uint32_t value)
{
    if (reg != 0)
        r->regs[reg] = value;
}

/* register_read() lê o valor armazenado no registrador especificado em um certo
 * banco de registradores, também passados por parâmetro. Se o registrador for o 0
 * (registrador r0), o valor 0 é retornado pois ele tem armazenado o valor 0 em
 * qualquer situação, caso contrário o banco de registradores é acessado. Tem
 * como retorno o valor armazenado no registrador em uint32_t.
 */
uint32_t register_read(register_bank *r, uint8_t reg)
{
    if (reg == 0)
        return 0;
    return r->regs[reg];
}

/* register_write_pc() escreve o valor passado como parâmetro no Program Counter
 * (PC) de um certo banco de registradores, também passado por parâmetro.
 */
void register_write_pc(register_bank *r, uint32_t pc)
{
    r->pc = pc;
}

/* register_read_pc() lê o valor armazenado no Program Counter (PC) de um certo
 * banco de registradores, também passado por parâmetro. Tem como retorno o valor
 * armazenado no PC em uint32_t.
 */
uint32_t register_read_pc(register_bank *r)
{
    return r->pc;
}

/* register_write_ir() escreve o valor passado como parâmetro no Instruction
 * Register (IR) de um certo banco de registradores, também passado por parâmetro.
 */
void register_write_ir(register_bank *r, uint32_t instruction)
{
    r->ir = instruction;
}

/* register_read_ir() lê o valor armazenado no Instruction Register (IR) de um
 * certo banco de registradores, também passado por parâmetro. Tem como retorno o
 * valor armazenado no IR em uint32_t.
 */
uint32_t register_read_ir(register_bank *r)
{
    return r->ir;
}