#include "leitor_sb.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Recebe um caminho e carrega os conteúdos do arquivo deste caminho para um
 * buffer string. Retorna um ponteiro para esse buffer ou NULL caso a operação
 * falhou.
 *
 * Importante: usuários do buffer devem liberá-lo após o uso.
 */
char *arquivo_para_string(char caminho[])
{
    FILE *f = fopen(caminho, "rb");
    if (!f)
    {
        perror("Erro ao abrir arquivo");
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    if (tamanho == -1L)
    {
        perror("Erro ao abrir arquivo");
        return 0;
    }
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(sizeof(char) * (tamanho + 1));
    if (!buffer)
    {
        printf("Nao ha memoria suficiente para carregar este arquivo!");
        return 0;
    }
    fread(buffer, sizeof *buffer, tamanho, f);
    buffer[tamanho] = '\0';

    fclose(f);

    return buffer;
}

// scanner

token scanner_faz_token(scanner *s, token_tipo tipo)
{
    token t;
    t.tipo = tipo;
    t.comp = (int)(s->atual - s->inicio);
    t.inicio = s->inicio;
    return t;
}

token scanner_faz_erro(scanner *s)
{
    token t;
    t.tipo = TOKEN_ERRO;
    t.comp = 0;
    t.inicio = "";
    fprintf(stderr, "ERRO!\n");
    return t;
}

char scanner_avancar(scanner *s)
{
    if (s->fim <= s->atual)
    {
        return '\0';
    }

    char ch = *(s->atual++);
    return ch;
}

char scanner_ver(scanner *s)
{
    if (s->fim <= s->atual)
    {
        return '\0';
    }

    return *s->atual;
}

char eh_numerico(char ch)
{
    return '0' <= ch && ch <= '9';
}

char eh_hex(char ch)
{
    return ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F');
}

char eh_alfabetico(char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

char eh_alfanumerico(char ch)
{
    return eh_alfabetico(ch) || eh_numerico(ch) || ch == '_';
}

void scanner_avanca_espaco(scanner *s)
{
    while (isspace(scanner_ver(s)))
    {
        scanner_avancar(s);
    }
}

token scanner_numero(scanner *s)
{
    char ch = scanner_ver(s);

    if (ch == 'x')
    {
        // come o x
        scanner_avancar(s);

        // o próximo tem que ser número
        ch = scanner_avancar(s);
        if (!eh_hex(ch))
        {
            scanner_faz_erro(s);
        }

        while (eh_hex(scanner_ver(s)))
        {
            scanner_avancar(s);
        }
    }
    else
    {
        while (eh_numerico(scanner_ver(s)))
        {
            scanner_avancar(s);
        }
    }

    return scanner_faz_token(s, TOKEN_NUMERO);
}

token scanner_identificador(scanner *s)
{
    while (eh_alfanumerico(scanner_ver(s)))
    {
        scanner_avancar(s);
    }

    return scanner_faz_token(s, TOKEN_NOME);
}

token prox_token(scanner *s)
{
    s->inicio = s->atual;
    if (s->atual >= s->fim)
    {
        return scanner_faz_token(s, TOKEN_EOF);
    }

    char ch = scanner_avancar(s);
    switch (ch)
    {

    // caracteres de um token
    case ':':
        return scanner_faz_token(s, TOKEN_DOIS_PONTOS);
    case '%':
        return scanner_faz_token(s, TOKEN_PORCENTO);

    case '(':
        return scanner_faz_token(s, TOKEN_PAREN_ESQ);

    case ')':
        return scanner_faz_token(s, TOKEN_PAREN_DIR);

    case '-':
        return scanner_faz_token(s, TOKEN_PAREN_DIR);

    case ',':
        return scanner_faz_token(s, TOKEN_VIRGULA);

    // espaço em branco
    case '\n':
        scanner_avanca_espaco(s);
        return scanner_faz_token(s, TOKEN_LINHA);
    case '\r':
    case '\t':
    case ' ':
        scanner_avanca_espaco(s);
        return prox_token(s);

    // comentário
    case '#':
        while ((ch = scanner_avancar(s)) && ch != '\n')
        {
        }
        return prox_token(s);

    default:
    {
        // numeros
        if (eh_numerico(ch))
        {
            return scanner_numero(s);
        }

        // identificadores
        if (eh_alfabetico(ch))
        {
            return scanner_identificador(s);
        }

        // etc
        return scanner_faz_erro(s);
    }
    }
}

void tokeniza(char *src, char *fim)
{
    scanner s;
    s.atual = src;
    s.fim = fim;
    s.inicio = src;

    for (;;)
    {
        token t = prox_token(&s);
        if (t.tipo == TOKEN_EOF || t.tipo == TOKEN_ERRO)
        {
            break;
        }
    }
}

// [PORCENTO, IDENT (add), DOIS_PONTOS, NUM (5), IDENT (mul), DOIS PONTOS, NUM (50), ..., PORCENTO]