#ifndef FUNC_UNIT_H
#define FUNC_UNIT_H

#include <stdlib.h>
#include <stdint.h>

#include "bus.h"
#include "opcode.h"
#include "sys_bus.h"

struct bus;
typedef struct bus bus;

struct sys_bus;
typedef struct sys_bus sys_bus;

typedef struct uf
{
    uint32_t instruction;
    int32_t res;
    int32_t res2;
} uf;

void uf_load_instruction(uf *u, uint32_t instruction);
void uf_load_ops(uf *u, bus *b, sys_bus *sb);
void uf_write_res(uf *u, bus *b, sys_bus *sb);

#endif