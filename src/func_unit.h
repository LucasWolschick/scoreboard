#ifndef FUNC_UNIT_H
#define FUNC_UNIT_H

#include <stdlib.h>
#include <stdint.h>

#include "bus.h"
#include "opcode.h"

struct bus;
typedef struct bus bus;

typedef struct uf
{
    uint32_t instruction;
    int32_t res;
    int32_t res2;
} uf;

void uf_load_instruction(uf *u, uint32_t instruction);
void uf_load_ops(uf *u, bus *b);
void uf_write_res(uf *u, bus *b);

#endif