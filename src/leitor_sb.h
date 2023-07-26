#ifndef LEITOR_SB_H
#define LEITOR_SB_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "instrucao.h"

enum token_tipo
{
    // um caractere
    TOKEN_DOIS_PONTOS,
    TOKEN_PORCENTO,
    TOKEN_VIRGULA,
    TOKEN_PAREN_ESQ,
    TOKEN_PAREN_DIR,
    TOKEN_MENOS,

    // múltiplos caracteres
    TOKEN_NOME,
    TOKEN_NUMERO,
    TOKEN_REGISTRADOR,

    // especiais
    TOKEN_EOF,
    TOKEN_ERRO
};
typedef enum token_tipo token_tipo;

struct token
{
    token_tipo tipo;
    int comp;
    const char *inicio;
    int valor;
    int linha;
};
typedef struct token token;

struct scanner
{
    char *atual;
    char *inicio;
    char *fim;
    int linha;
};
typedef struct scanner scanner;

struct config
{
    // num de unidades funcionais
    int n_uf_add;
    int n_uf_mul;
    int n_uf_int;

    // ciclos por instrucao
    int ck_ld;
    int ck_mul;
    int ck_add;
    int ck_sub;
    int ck_div;
};
typedef struct config config;

char *arquivo_para_string(char caminho[]);
token scanner_faz_token(scanner *s, token_tipo tipo);
token scanner_faz_erro(scanner *_s);
char scanner_avancar(scanner *s);
char scanner_ver(scanner *s);
void scanner_avanca_espaco(scanner *s);
token scanner_numero(scanner *s);
token scanner_identificador(scanner *s);
token prox_token_r(scanner *s);
void token_lexema(token t);
token prox_token(scanner *s);
token ve_token(scanner *s);
token espera_token(scanner *s, token_tipo tipo);
bool compara_token(token t, char *str);
void atrib_uf(scanner *s, config *c);
void atrib_inst(scanner *s, config *c);
void bloco_configuracao(scanner *s, config *c);
config configuracao(scanner *s);
inst instrucao(scanner *s);
void le_sb(char *src, char *fim, config *config_saida, inst **instrucoes_saida, int *n_instrucoes);

#endif
