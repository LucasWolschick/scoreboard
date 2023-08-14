#include <stdbool.h>

#include "cpu.h"
#include "opcode.h"
#include "memory.h"
#include "register.h"
#include "func_unit.h"
#include "scoreboarding.h"
#include "parser.h"

// TODO: usar o IR
// TODO: buffer de instruções buscadas
// TODO: janela pós-fetch
// TODO: controlar a largura máxima de banda do barramento na hora de fazer a escrita

cpu *cpu_init(bus *b, config cfg, int n_instructions)
{
    scoreboard_status *status = malloc(sizeof(scoreboard_status));
    status->inst = calloc(sizeof(instruction_status), n_instructions);
    for (int i = 0; i < n_instructions; i++)
    {
        status->inst[i].st = STAGE_FETCH;
        status->inst[i].uf = -1;
        for (stage s = STAGE_FETCH; s <= STAGE_DONE; s++)
        {
            status->inst[i].when[s] = -1;
        }
    }

    status->regs = calloc(sizeof(reg_status), N_REGISTERS);
    for (int r = 0; r < N_REGISTERS; r++)
    {
        status->regs[r].uf = -1;
    }

    int n_ufs = cfg.n_uf_add + cfg.n_uf_int + cfg.n_uf_mul;
    status->uf = calloc(sizeof(uf_status), n_ufs);
    for (int u = 0; u < n_ufs; u++)
    {
        if (u < cfg.n_uf_add)
        {
            status->uf[u].type = FU_ADD;
        }
        else if (u < cfg.n_uf_add + cfg.n_uf_int)
        {
            status->uf[u].type = FU_INT;
        }
        else
        {
            status->uf[u].type = FU_MUL;
        }
    }

    cpu *c = malloc(sizeof(cpu));
    c->ck = 0;
    c->status = status;
    c->bus = b;
    c->stall = false;
    c->stop = false;
    c->cfg = cfg;
    c->n_instructions = n_instructions;

    return c;
}

void cpu_destroy(cpu *c)
{
    free(c->status->inst);
    free(c->status->regs);
    free(c->status->uf);
    free(c->status);
    free(c);
}

void fetch(cpu *c)
{
    if (c->stall)
    {
        // não busca ninguém
        return;
    }

    int pc = bus_read_pc(c->bus);

    // carregar a instrução da memória e colocar na tabela inst
    uint32_t instruction = bus_read_memory(c->bus, pc);
    c->status->inst[pc].instruction = instruction;
    c->status->inst[pc].st = STAGE_ISSUE;

    for (stage s = STAGE_FETCH; s <= STAGE_DONE; s++)
    {
        c->status->inst[pc].when[s] = -1;
    }
    c->status->inst[pc].when[STAGE_FETCH] = c->ck;

    // incrementar contador
    // tratamento especial para jumps
    int opcode = instruction >> 26;
    int address = instruction & 0x3FFFFFF;

    switch (opcode)
    {
    case OP_BLT:
    case OP_BEQ:
    case OP_BGT:
    case OP_BNE:
        c->stall = true;
        break;

    case OP_J:
        bus_write_pc(c->bus, address);
        break;

    default:
        bus_write_pc(c->bus, pc + 1);
        break;
    }
}

void issue(cpu *c)
{
    // para cada instrução nesta etapa...
    for (int i = 100; i < c->n_instructions; i++)
    {
        instruction_status inst = c->status->inst[i];
        if (inst.st != STAGE_ISSUE)
        {
            continue;
        }

        // decodifica
        int opcode = inst.instruction >> 26;
        int rs, rt, rd, extra, imm, address;

        rs = (inst.instruction >> 21) & 0x1F;
        rt = (inst.instruction >> 16) & 0x1F;
        rd = (inst.instruction >> 11) & 0x1F;
        extra = inst.instruction & 0x7FF;
        imm = inst.instruction & 0xFFFF;
        address = inst.instruction & 0x1FFFFFF;

        int i_type, d, s1, s2;
        switch (opcode)
        {
        case OP_ADD:
        case OP_SUB:
            d = rd;
            s1 = rs;
            s2 = rt;
            i_type = FU_ADD;
            break;

        case OP_MUL:
        case OP_DIV:
            d = rd;
            s1 = rs;
            s2 = rt;
            i_type = FU_MUL;
            break;

        case OP_ADDI:
        case OP_SUBI:
            d = rt;
            s1 = rs;
            s2 = imm;
            i_type = FU_ADD;
            break;

        case OP_AND:
        case OP_OR:
            d = rd;
            s1 = rs;
            s2 = rt;
            i_type = FU_INT;
            break;

        case OP_NOT:
            d = rd;
            s1 = rs;
            s2 = -1;
            i_type = FU_INT;
            break;

        case OP_BLT:
        case OP_BGT:
        case OP_BEQ:
        case OP_BNE:
            d = -1;
            s1 = rs;
            s2 = rt;
            i_type = FU_INT;
            break;

        case OP_J:
            d = -1;
            s1 = -1;
            s2 = -1;
            i_type = FU_INT;
            break;

        case OP_LW:
        case OP_SW:
            d = rt;
            s1 = rs;
            s2 = imm;
            i_type = FU_INT;
            break;

        case OP_EXIT:
            d = -1;
            s1 = -1;
            s2 = -1;
            i_type = FU_INT;
            break;
        }

        // verifica se o registrador-resultado não está marcado para receber
        // um valor de outra UF
        if (d != -1 && c->status->regs[d].uf != -1)
        {
            continue;
        }

        // encontra uma UF que está vaga
        int uf = -1;
        for (int i = 0; i < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); i++)
        {
            if (c->status->uf[i].type == i_type && !c->status->uf[i].busy)
            {
                uf = i;
                break;
            }
        }

        if (uf == -1)
        {
            continue;
        }

        // nesse caso, a gente emite
        c->status->regs[d].uf = uf;

        c->status->uf[uf].busy = true;
        c->status->uf[uf].op = opcode;
        c->status->uf[uf].fi = d;
        c->status->uf[uf].fj = s1;
        c->status->uf[uf].fk = s2;
        c->status->uf[uf].qj = c->status->regs[s1].uf;
        c->status->uf[uf].qk = c->status->regs[s2].uf;
        c->status->uf[uf].rj = c->status->regs[s1].uf == -1;
        c->status->uf[uf].rk = c->status->regs[s2].uf == -1;

        c->status->inst[i].when[STAGE_ISSUE] = c->ck;
        c->status->inst[i].st = STAGE_READ_OPERANDS;
        c->status->inst[i].uf = uf;

        // carrega a instrução na uf
        bus_func_unit_load_instruction(c->bus, uf, inst.instruction);

        // apenas uma instrução é emitida por ciclo
        break;
    }
}

void read_operands(cpu *c)
{
    // para cada instrução...
    for (int i = 0; i < c->n_instructions; i++)
    {
        // se a instrução está nesta etapa...
        instruction_status inst = c->status->inst[i];
        if (inst.st != STAGE_READ_OPERANDS)
        {
            continue;
        }

        // se os dois operandos estão prontos...
        if (c->status->uf[inst.uf].rj && c->status->uf[inst.uf].rk)
        {
            // manda executar
            c->status->uf[inst.uf].rj = false;
            c->status->uf[inst.uf].rk = false;

            // lê os operandos e despacha para a UF
            bus_func_unit_load_ops(c->bus, inst.uf);

            c->status->inst[i].st = STAGE_EXECUTION_COMPLETE;
            c->status->inst[i].when[STAGE_READ_OPERANDS] = c->ck;
        }
    }
}

void execution_complete(cpu *c)
{
    // para cada instrução...
    for (int i = 100; i < c->n_instructions; i++)
    {
        // se a instrução está nesta etapa...
        instruction_status inst = c->status->inst[i];
        if (inst.st != STAGE_EXECUTION_COMPLETE)
        {
            continue;
        }

        // a UF está pronta se o clock atual - o clock em que ela concluiu a leitura de operandos
        // for maior ou igual ao tempo de execução dela
        if (c->ck - inst.when[STAGE_READ_OPERANDS] >= c->cfg.ck_instruction[c->status->uf[inst.uf].op])
        {
            // manda escrever
            c->status->inst[i].st = STAGE_WRITE_RESULTS;
            c->status->inst[i].when[STAGE_EXECUTION_COMPLETE] = c->ck;
        }
    }
}

void write_results(cpu *c)
{
    for (int i = 100; i < c->n_instructions; i++)
    {
        instruction_status inst = c->status->inst[i];

        if (inst.st != STAGE_WRITE_RESULTS)
        {
            continue;
        }

        // verifica a condição
        bool condition = true;
        for (int f = 0; f < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); f++)
        {
            if (!((c->status->uf[f].fj != c->status->uf[inst.uf].fi || c->status->uf[f].rj == false) ||
                  (c->status->uf[f].fk != c->status->uf[inst.uf].fi || c->status->uf[f].rk == false)))
            {
                condition = false;
                break;
            }
        }

        if (!condition)
        {
            continue;
        }

        // escreve o resultado
        for (int f = 0; f < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); f++)
        {
            if (c->status->uf[f].qj == inst.uf)
            {
                c->status->uf[f].rj = true;
            }
            if (c->status->uf[f].qk == inst.uf)
            {
                c->status->uf[f].rk = true;
            }
        }

        bus_func_unit_write_res(c->bus, inst.uf);

        // resolve o stall no caso da instrução ser um branch condicional
        int opcode = inst.instruction >> 26;
        switch (opcode)
        {
        case OP_BLT:
        case OP_BEQ:
        case OP_BGT:
        case OP_BNE:
            c->stall = false;
            break;
        }

        // limpa
        c->status->regs[c->status->uf[inst.uf].fi].uf = -1;
        c->status->uf[inst.uf] = (uf_status){0};

        c->status->inst[i].st = STAGE_DONE;
        c->status->inst[i].when[STAGE_WRITE_RESULTS] = c->ck;
    }
}

void pipeline(cpu *c)
{
    if (!c->stop)
    {
        fetch(c);
        issue(c);
        read_operands(c);
        execution_complete(c);
        write_results(c);
    }
}