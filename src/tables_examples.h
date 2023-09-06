#ifndef TABLES_EXAMPLES_H
#define TABLES_EXAMPLES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bus.h"
#include "cpu.h"
#include "scoreboard.h"

void print_tables_instruction(scoreboard *sb);
void print_tables_ufs(scoreboard *sb);
void print_tables_regs(scoreboard *sb);
void print_tables(scoreboard *sb);

#endif