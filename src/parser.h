#ifndef SCANNER_H
#define SCANNER_H

#include "vector.h"
#include "opcode.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

typedef struct scanner
{
    const char *start;
    const char *current;
    int line;
} scanner;

typedef enum token_type
{
    // um caractere
    TOKEN_COMMA,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_MINUS,
    TOKEN_COLON,

    // dois caracteres
    TOKEN_START_COMMENT,
    TOKEN_END_COMMENT,

    // múltiplos caracteres
    TOKEN_MNEMONIC,
    TOKEN_REGISTER,
    TOKEN_NUMBER,
    TOKEN_SECTION,
    TOKEN_IDENTIFIER,

    // especiais
    TOKEN_EOF,
    TOKEN_ERRO,
} token_type;

typedef struct token
{
    token_type type;
    const char *lexeme;
    int len;
    int value;
    int line;
} token;

typedef enum section_name
{
    SECTION_DATA,
    SECTION_TEXT
} section_name;

char next_char(scanner *s);

char peek_char(const scanner *s);

void skip_whitespace(scanner *s);

token scan_number(scanner *s);

bool cmp_token(token t, const char *s);

token make_token(const scanner *s, token_type type);

token make_error(const scanner *s);

void print_token(token t, FILE *f);

const char *token_type_str(token_type type);

token scan_section(scanner *s);

token scan_identifier(scanner *s);

token next_token(scanner *s);

token peek_token(scanner *s);

token expect_token(scanner *s, token_type type);

uint32_t instruction(scanner *s);

void decl_uf(scanner *s, config *c);

void decl_inst(scanner *s, config *c);

config comment(scanner *s);

void data_entry(scanner *s, vector *v);

vector data_section(scanner *s);

vector text_section(scanner *s);

void parse(const char *src, vector *data_vector, vector *inst_vector, config *config_out);

#endif