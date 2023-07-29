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
    TOKEN_VIRGULA,
    TOKEN_PAREN_ESQ,
    TOKEN_PAREN_DIR,
    TOKEN_MENOS,

    // múltiplos caracteres
    TOKEN_NOME,
    TOKEN_NUMERO,
    TOKEN_SECAO,
    TOKEN_REGISTRADOR,
    TOKEN_COMECA_COMENTARIO,
    TOKEN_TERMINA_COMENTARIO,

    // especiais
    TOKEN_EOF,
    TOKEN_ERRO,
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
    int ck_instrucoes[OP_EXIT];
};
typedef struct config config;

char *arquivo_para_string(char caminho[]);
token scanner_faz_token(scanner *s, token_tipo tipo);
token scanner_faz_erro(scanner *s);
char scanner_avancar(scanner *s);
char scanner_ver(scanner *s);
void scanner_avanca_espaco(scanner *s);
token scanner_numero(scanner *s);
token scanner_secao(scanner *s);
token scanner_identificador(scanner *s);
token scanner_registrador(scanner *s);
const char *tipo_token_string(token_tipo tipo);
token prox_token_r(scanner *s);
void token_lexema(token t);
token prox_token(scanner *s);
token ve_token(scanner *s);
token espera_token(scanner *s, token_tipo tipo);
bool compara_token(token t, char *str);
void atrib_uf(scanner *s, config *c);
void atrib_inst(scanner *s, config *c);
void bloco_configuracao(scanner *s, config *c);
config config_novo(void);
opcode_instrucao mnemonico(scanner *s);
config configuracao(scanner *s);
inst instrucao(scanner *s);
void dados(scanner *s);
void instrucoes(scanner *s, int *num_instrucoes_saida, inst **instrucoes_lidas_saida);
void le_sb(char *src, char *fim, config *config_saida, inst **instrucoes_saida, int *n_instrucoes);

#endif
