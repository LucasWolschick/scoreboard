#ifndef LEITOR_SB_H
#define LEITOR_SB_H

enum token_tipo
{
    // um caractere
    TOKEN_LINHA,
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
};
typedef struct token token;

struct scanner
{
    char *atual;
    char *inicio;
    char *fim;
};
typedef struct scanner scanner;

char *arquivo_para_string(char caminho[]);
void tokeniza(char *src, char *fim);

#endif