#ifndef TABELA_HASH_H
#define TABELA_HASH_H

#include <stdbool.h>

#define FATOR_DE_CARGA_HASH 0.75

typedef struct bucket_no bucket_no;
struct bucket_no
{
    const char *chave;
    int valor;
    bucket_no *prox;
};

typedef struct tabela_hash tabela_hash;
struct tabela_hash
{
    bucket_no **buckets;
    unsigned int comprimento;
    unsigned int itens;
};

unsigned int hash(const char *string);

bucket_no *encontra_bucket(bucket_no *raiz, const char *chave);

tabela_hash *tabela_hash_novo(void);
void tabela_hash_destroi(tabela_hash *tabela);
void tabela_hash_cresce(tabela_hash *tabela);
bool tabela_hash_get(tabela_hash *tabela, const char *chave, int *destino);
bool tabela_hash_set(tabela_hash *tabela, const char *chave, int valor);
bool tabela_hash_remove(tabela_hash *tabela, const char *chave);
tabela_hash *tabela_hash_copia(tabela_hash *tabela);

#endif
