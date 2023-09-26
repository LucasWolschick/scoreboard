#ifndef TABLES_EXAMPLES_H
#define TABLES_EXAMPLES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "register.h"
#include "cpu.h"
#include "scoreboard.h"

void print_tables_instruction(scoreboard *sb);
void print_tables_ufs(scoreboard *sb, register_bank *regs);
void print_tables_regs(scoreboard *sb);
void print_tables_regs_f(scoreboard *sb, register_bank *r);
void print_tables(scoreboard *sb, register_bank *regs);
void print_memory(memory *m);

#endif