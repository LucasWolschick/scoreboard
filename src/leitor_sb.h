#ifndef LEITOR_SB_H
#define LEITOR_SB_H

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
};
typedef struct token token;

struct scanner
{
    char *atual;
    char *inicio;
    char *fim;
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
void tokeniza(char *src, char *fim);
void le_sb(char *src, char *fim);

#endif