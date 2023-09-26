#include <stdbool.h>

#include "cpu.h"
#include "opcode.h"
#include "memory.h"
#include "register.h"
#include "func_unit.h"
#include "scoreboard.h"
#include "parser.h"
#include "bus.h"
#include "sys_bus.h"

/*
    TODO: controlar a largura máxima de banda do barramento na hora de fazer a escrita

        Based on its  own data structure,  the scoreboard controls  the instruction pro-
    gression from one step to the next by communicating with the functional units. There
    is a small complication, however.  There are only a limited number of source operand
    buses and result buses to the register file,  which represents a structural  hazard.
    The scoreboard must guarantee that the number of functional units allowed to proceed
    into steps 2 (read operands)  and  4 (write results) does  not exceed the number  of
    buses available.  We will not go into further detail on this,  other than to mention
    that the CDC 6600  solved this problem by grouping the  16 functional units together
    into four groups and supplying a set of buses,  called data trunks,  for each group.
    Only one unit in a group could read its operands or write its result during a clock.

    ~ HENNESSY, J; PATTERSON, H. Computer Architecture: A Quantitative Approach. (p. C-75)
*/

cpu *cpu_init(bus *b, sys_bus *sb, config cfg, int n_instructions)
{
    cpu *c = malloc(sizeof(cpu));
    c->ck = 0;
    c->bus = b;
    c->sys_bus = sb;
    c->pipeline_status = PIPELINE_RUNNING;
    c->cfg = cfg;
    c->n_instructions = n_instructions;
    c->fetch_buffer = inst_buffer_init(n_instructions);

    return c;
}

void cpu_destroy(cpu *c)
{
    inst_buffer_destroy(c->fetch_buffer);
    free(c);
}

int snprint_instruction(uint32_t instruction, char *buffer, size_t bufsz)
{
    uint32_t opcode = instruction >> 26;
    uint32_t rs, rt, rd, imm, address;

    rs = (instruction >> 21) & 0x1F;
    rt = (instruction >> 16) & 0x1F;
    rd = (instruction >> 11) & 0x1F;
    // extra = instruction & 0x7FF;
    imm = instruction & 0xFFFF;
    if (imm & 0x8000)
    {
        imm |= 0xFFFF0000;
    }
    address = instruction & 0x1FFFFFF;

    switch (opcode)
    {
    case OP_ADD:
        return snprintf(buffer, bufsz, "add r%d, r%d, r%d", rd, rs, rt);
    case OP_ADDI:
        return snprintf(buffer, bufsz, "addi r%d, r%d, %d", rt, rs, imm);
    case OP_SUB:
        return snprintf(buffer, bufsz, "sub r%d, r%d, r%d", rd, rs, rt);
    case OP_SUBI:
        return snprintf(buffer, bufsz, "subi r%d, r%d, %d", rt, rs, imm);
    case OP_MUL:
        return snprintf(buffer, bufsz, "mul r%d, r%d, r%d", rd, rs, rt);
    case OP_DIV:
        return snprintf(buffer, bufsz, "div r%d, r%d, r%d", rd, rs, rt);
    case OP_AND:
        return snprintf(buffer, bufsz, "and r%d, r%d, r%d", rd, rs, rt);
    case OP_OR:
        return snprintf(buffer, bufsz, "or r%d, r%d, r%d", rd, rs, rt);
    case OP_NOT:
        return snprintf(buffer, bufsz, "not r%d, r%d", rd, rs);
    case OP_BLT:
        return snprintf(buffer, bufsz, "blt r%d, r%d, %d", rs, rt, imm);
    case OP_BGT:
        return snprintf(buffer, bufsz, "bgt r%d, r%d, %d", rs, rt, imm);
    case OP_BEQ:
        return snprintf(buffer, bufsz, "beq r%d, r%d, %d", rs, rt, imm);
    case OP_BNE:
        return snprintf(buffer, bufsz, "bne r%d, r%d, %d", rs, rt, imm);
    case OP_J:
        return snprintf(buffer, bufsz, "j %d", address);
    case OP_LW:
        return snprintf(buffer, bufsz, "lw r%d, %d(r%d)", rt, imm, rs);
    case OP_SW:
        return snprintf(buffer, bufsz, "sw r%d, %d(r%d)", rt, imm, rs);
    case OP_EXIT:
        return snprintf(buffer, bufsz, "exit");
    }
    return 0;
}

void fetch(cpu *c, scoreboard *board)
{
    if (c->pipeline_status != PIPELINE_RUNNING)
    {
        // não busca ninguém

        if (c->pipeline_status == PIPELINE_UNSTALL)
        {
            c->pipeline_status = PIPELINE_RUNNING;
        }

        return;
    }

    if (inst_buffer_full(c->fetch_buffer))
    {
        // não busca nada
        return;
    }

    int pc = bus_read_pc(c->bus);

    // carregar a instrução da memória e colocar na tabela inst
    bus_load_pc_ir(c->bus, c->sys_bus);
    uint32_t instruction = bus_read_ir(c->bus);

    instruction_status inst;
    inst.instruction = instruction;
    inst.st = STAGE_ISSUE;
    inst.uf = -1;
    for (stage s = STAGE_FETCH; s <= STAGE_DONE; s++)
    {
        inst.when[s] = -1;
    }
    inst.when[STAGE_FETCH] = c->ck;
    uint32_t iid = scoreboard_add_instruction(board, inst);
    inst_buffer_queue(c->fetch_buffer, iid);

    // incrementar contador
    // tratamento especial para jumps
    uint32_t opcode = instruction >> 26;
    uint32_t address = instruction & 0x3FFFFFF;

    switch (opcode)
    {
    case OP_BLT:
    case OP_BEQ:
    case OP_BGT:
    case OP_BNE:
        c->pipeline_status = PIPELINE_STALL;
        break;
    case OP_EXIT:
        c->pipeline_status = PIPELINE_STOP;
        break;

    case OP_J:
        bus_write_pc(c->bus, address);
        break;

    default:
        bus_write_pc(c->bus, pc + 4);
        break;
    }
}

void issue(cpu *c, scoreboard *board)
{
    // tenta emitir uma única instrução
    if (inst_buffer_size(c->fetch_buffer) == 0)
    {
        return;
    }

    int i = inst_buffer_peek(c->fetch_buffer);
    instruction_status inst = *bus_sb_get_instruction(c->bus, i);

    // decodifica
    uint32_t opcode = inst.instruction >> 26;
    uint32_t rs, rt, rd, imm;

    rs = (inst.instruction >> 21) & 0x1F;
    rt = (inst.instruction >> 16) & 0x1F;
    rd = (inst.instruction >> 11) & 0x1F;
    // extra = inst.instruction & 0x7FF;
    imm = inst.instruction & 0xFFFF;
    if (imm & 0x8000)
    {
        imm |= 0xFFFF0000;
    }
    // address = inst.instruction & 0x1FFFFFF;

    int i_type = 0, d = 0, s1 = 0, s2 = 0;
    bool is_imm = false;
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
        is_imm = true;
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
        is_imm = true;
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
    if (d != -1 && bus_sb_get_register_status(c->bus, d)->uf != -1)
    {
        return;
    }

    // encontra uma UF que está vaga
    int uf = -1;
    for (int i = 0; i < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); i++)
    {
        if (bus_sb_get_func_unit_status(c->bus, i)->type == i_type && !bus_sb_get_func_unit_status(c->bus, i)->busy)
        {
            uf = i;
            break;
        }
    }

    if (uf == -1)
    {
        return;
    }

    // nesse caso, a gente emite
    if (d != -1)
        board->regs[d].uf = uf;

    uf_status *uf_s = &board->uf[uf];
    uf_s->busy = true;
    uf_s->op = opcode;
    uf_s->fi = d;
    uf_s->fj = s1;
    uf_s->fk = s2;
    uf_s->qj = s1 != -1 ? bus_sb_get_register_status(c->bus, s1)->uf : -1;
    uf_s->rj = s1 == -1 || bus_sb_get_register_status(c->bus, s1)->uf == -1;
    if (!is_imm)
    {
        uf_s->qk = s2 != -1 ? bus_sb_get_register_status(c->bus, s2)->uf : -1;
        uf_s->rk = s2 == -1 || bus_sb_get_register_status(c->bus, s2)->uf == -1;
    }
    else
    {
        uf_s->qk = -1;
        uf_s->rk = true;
    }

    instruction_status *inst_s = &board->inst[i];
    inst_s->when[STAGE_ISSUE] = c->ck;
    inst_s->st = STAGE_READ_OPERANDS;
    inst_s->uf = uf;

    inst_buffer_dequeue(c->fetch_buffer);

    // carrega a instrução na uf
    bus_func_unit_load_instruction(c->bus, uf, inst.instruction);
}

void read_operands(cpu *c, scoreboard *board)
{
    // para cada instrução...
    size_t n_insts = bus_sb_n_instructions(c->bus);
    for (int i = 0; i < n_insts; i++)
    {
        // se a instrução está nesta etapa...
        instruction_status inst = *bus_sb_get_instruction(c->bus, i);
        if (inst.st != STAGE_READ_OPERANDS)
        {
            continue;
        }

        // se os dois operandos estão prontos...
        uf_status *uf_s = bus_sb_get_func_unit_status(c->bus, inst.uf);
        if (uf_s->rj && uf_s->rk)
        {
            uf_s = &board->uf[inst.uf];

            // manda executar
            uf_s->rj = false;
            uf_s->rk = false;

            // limpa ufs origem
            uf_s->qj = -1;
            uf_s->qk = -1;

            // lê os operandos e despacha para a UF
            bus_func_unit_load_ops(c->bus, inst.uf, c->sys_bus);

            instruction_status *inst_s = &board->inst[i];
            inst_s->st = STAGE_EXECUTION_COMPLETE;
            inst_s->when[STAGE_READ_OPERANDS] = c->ck;
        }
    }
}

void execution_complete(cpu *c, scoreboard *board)
{
    // para cada instrução...
    size_t n_insts = bus_sb_n_instructions(c->bus);
    for (int i = 0; i < n_insts; i++)
    {
        // se a instrução está nesta etapa...
        instruction_status inst = *bus_sb_get_instruction(c->bus, i);
        if (inst.st != STAGE_EXECUTION_COMPLETE)
        {
            continue;
        }

        // a UF está pronta se o clock atual - o clock em que ela concluiu a leitura de operandos
        // for maior ou igual ao tempo de execução dela
        if (c->ck - inst.when[STAGE_READ_OPERANDS] >= c->cfg.ck_instruction[bus_sb_get_func_unit_status(c->bus, inst.uf)->op])
        {
            // manda escrever
            instruction_status *inst_s = &board->inst[i];
            inst_s->st = STAGE_WRITE_RESULTS;
            inst_s->when[STAGE_EXECUTION_COMPLETE] = c->ck;
        }
    }
}

void write_results(cpu *c, scoreboard *board)
{
    size_t n_insts = bus_sb_n_instructions(c->bus);
    for (int i = 0; i < n_insts; i++)
    {
        instruction_status inst = *bus_sb_get_instruction(c->bus, i);

        if (inst.st != STAGE_WRITE_RESULTS)
        {
            continue;
        }

        uf_status *uf_uf_status = bus_sb_get_func_unit_status(c->bus, inst.uf);

        // verifica a condição
        bool condition = true;

        for (int f = 0; f < c->bus->board->n_ufs; f++)
        {
            uf_status *f_uf_status = bus_sb_get_func_unit_status(c->bus, f);
            if (uf_uf_status->fi != -1)
            {
                if (!((f_uf_status->fj != uf_uf_status->fi || f_uf_status->rj == false) &&
                      (f_uf_status->fk != uf_uf_status->fi || f_uf_status->rk == false)))
                {
                    condition = false;
                    break;
                }
            }
        }

        if (!condition)
        {
            continue;
        }

        // escreve o resultado
        // for (int f = 0; f < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); f++)
        // {
        //     uf_status *f_uf_status = &board->uf[f];

        //     if (inst.uf != -1 && f_uf_status->qj == inst.uf)
        //     {
        //         f_uf_status->rj = true;
        //     }
        //     if (inst.uf != -1 && f_uf_status->qk == inst.uf)
        //     {
        //         f_uf_status->rk = true;
        //     }
        // }

        bus_func_unit_write_res(c->bus, inst.uf, c->sys_bus);

        // resolve o stall no caso da instrução ser um branch condicional
        int opcode = inst.instruction >> 26;
        switch (opcode)
        {
        case OP_BLT:
        case OP_BEQ:
        case OP_BGT:
        case OP_BNE:
            c->pipeline_status = PIPELINE_UNSTALL;
            break;
        }

        // limpa
        // int reg_target = bus_sb_get_func_unit_status(c->bus, inst.uf)->fi;
        // if (reg_target != -1)
        //     board->regs[reg_target].uf = -1;

        // board->uf[inst.uf] = (uf_status){
        //     .busy = false,
        //     .op = -1,
        //     .type = bus_sb_get_func_unit_status(c->bus, inst.uf)->type,

        //     .fi = -1,
        //     .fj = -1,
        //     .fk = -1,

        //     .qj = -1,
        //     .qk = -1,

        //     .rj = false,
        //     .rk = false};

        // instruction_status *inst_s = &board->inst[i];
        // inst_s->st = STAGE_DONE;
        // inst_s->when[STAGE_WRITE_RESULTS] = c->ck;
    }
}

void write_results_2(cpu *c, scoreboard *board)
{
    size_t n_insts = bus_sb_n_instructions(c->bus);
    for (int i = 0; i < n_insts; i++)
    {
        instruction_status inst = *bus_sb_get_instruction(c->bus, i);

        if (inst.st != STAGE_WRITE_RESULTS)
        {
            continue;
        }

        uf_status *uf_uf_status = bus_sb_get_func_unit_status(c->bus, inst.uf);

        // verifica a condição
        bool condition = true;

        for (int f = 0; f < c->bus->board->n_ufs; f++)
        {
            uf_status *f_uf_status = bus_sb_get_func_unit_status(c->bus, f);
            if (uf_uf_status->fi != -1)
            {
                if (!((f_uf_status->fj != uf_uf_status->fi || f_uf_status->rj == false) &&
                      (f_uf_status->fk != uf_uf_status->fi || f_uf_status->rk == false)))
                {
                    condition = false;
                    break;
                }
            }
        }

        if (!condition)
        {
            continue;
        }

        // escreve o resultado
        for (int f = 0; f < (c->cfg.n_uf_add + c->cfg.n_uf_int + c->cfg.n_uf_mul); f++)
        {
            uf_status *f_uf_status = &board->uf[f];

            if (inst.uf != -1 && f_uf_status->qj == inst.uf)
            {
                f_uf_status->rj = true;
            }
            if (inst.uf != -1 && f_uf_status->qk == inst.uf)
            {
                f_uf_status->rk = true;
            }
        }

        // bus_func_unit_write_res(c->bus, inst.uf, c->sys_bus);

        // // resolve o stall no caso da instrução ser um branch condicional
        // int opcode = inst.instruction >> 26;
        // switch (opcode)
        // {
        // case OP_BLT:
        // case OP_BEQ:
        // case OP_BGT:
        // case OP_BNE:
        //     c->pipeline_status = PIPELINE_UNSTALL;
        //     break;
        // }

        // limpa
        int reg_target = bus_sb_get_func_unit_status(c->bus, inst.uf)->fi;
        if (reg_target != -1)
            board->regs[reg_target].uf = -1;

        board->uf[inst.uf] = (uf_status){
            .busy = false,
            .op = -1,
            .type = bus_sb_get_func_unit_status(c->bus, inst.uf)->type,

            .fi = -1,
            .fj = -1,
            .fk = -1,

            .qj = -1,
            .qk = -1,

            .rj = false,
            .rk = false};

        instruction_status *inst_s = &board->inst[i];
        inst_s->st = STAGE_DONE;
        inst_s->when[STAGE_WRITE_RESULTS] = c->ck;
    }
}

bool pipeline(cpu *c)
{
    scoreboard *next_board = scoreboard_copy(c->bus->board);

    write_results(c, next_board);
    execution_complete(c, next_board);
    read_operands(c, next_board);
    issue(c, next_board);
    fetch(c, next_board);
    write_results_2(c, next_board);

    scoreboard_destroy(c->bus->board);
    c->bus->board = next_board;

    c->ck++;

    // checa se a pipeline está vazia
    size_t n_insts = bus_sb_n_instructions(c->bus);
    bool empty = true;
    for (int i = 0; i < n_insts; i++)
    {
        instruction_status inst = *bus_sb_get_instruction(c->bus, i);
        if (inst.st != STAGE_DONE)
        {
            empty = false;
            break;
        }
    }

    return !(c->pipeline_status == PIPELINE_STOP && empty);
}