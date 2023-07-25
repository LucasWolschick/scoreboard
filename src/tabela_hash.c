#include "tabela_hash.h"

#include <stdlib.h>
#include <string.h>

unsigned int hash(const char *string)
{
    // Função hash FNV-1a
    // Fowler/Noll/Vo, 1991
    unsigned int hash = 2166136261;
    for (const char *c = string; *c != '\0'; c++)
    {
        hash = hash ^ *c;
        hash = hash * 16777619;
    }
    return hash;
}

bucket_no *encontra_bucket(bucket_no *raiz, const char *chave)
{
    if (!raiz)
        return NULL;

    if (strcmp(raiz->chave, chave) == 0)
        return raiz;

    return encontra_bucket(raiz->prox, chave);
}

tabela_hash *tabela_hash_novo(void)
{
    tabela_hash *tabela = malloc(sizeof *tabela);
    tabela->buckets = NULL;
    tabela->comprimento = 0;
    tabela->itens = 0;
    return tabela;
}

void tabela_hash_destroi(tabela_hash *tabela)
{
    if (tabela->buckets)
    {
        for (size_t i = 0; i < tabela->comprimento; i++)
        {
            bucket_no *ptr = tabela->buckets[i];
            while (ptr)
            {
                bucket_no *prox = ptr->prox;
                free(ptr->chave);
                free(ptr);
                ptr = prox;
            }
        }
    }
    tabela->buckets = NULL;
    tabela->comprimento = 0;
    tabela->itens = 0;
    free(tabela);
}

bool tabela_hash_get(tabela_hash *tabela, const char *chave, int *destino)
{
    unsigned int hash_chave = hash(chave);
    unsigned int indice = hash_chave % tabela->comprimento;
    bucket_no *bucket = encontra_bucket(tabela->buckets[indice], chave);
    if (bucket)
    {
        *destino = bucket->valor;
        return true;
    }
    else
    {
        return false;
    }
}

void tabela_hash_cresce(tabela_hash *tabela)
{
    tabela_hash *nova_tabela = tabela_hash_novo();
    nova_tabela->comprimento = tabela->comprimento * 2;
    if (nova_tabela->comprimento == 0)
    {
        nova_tabela->comprimento = 2;
    }
    nova_tabela->buckets = malloc(sizeof *nova_tabela->buckets * nova_tabela->comprimento);
    for (size_t i = 0; i < nova_tabela->comprimento; i++)
    {
        nova_tabela->buckets[i] = NULL;
    }

    for (size_t i = 0; i < tabela->comprimento; i++)
    {
        bucket_no *ptr = tabela->buckets[i];
        while (ptr)
        {
            tabela_hash_set(nova_tabela, ptr->chave, ptr->valor);
            ptr = ptr->prox;
        }
    }

    tabela_hash *antiga_tabela = malloc(sizeof *antiga_tabela);
    *antiga_tabela = *tabela;

    tabela->buckets = nova_tabela->buckets;
    tabela->comprimento = nova_tabela->comprimento;
    tabela->itens = nova_tabela->itens;

    tabela_hash_destroi(antiga_tabela);
}

bool tabela_hash_set(tabela_hash *tabela, const char *chave, int valor)
{
    if (tabela->itens + 1 > tabela->comprimento * FATOR_DE_CARGA_HASH)
    {
        tabela_hash_cresce(tabela);
    }

    unsigned int hash_chave = hash(chave);
    unsigned int indice_chave = hash_chave % tabela->comprimento;

    bucket_no *bucket_existente = encontra_bucket(tabela->buckets[indice_chave], chave);
    if (bucket_existente)
    {
        bucket_existente->valor = valor;
        return true;
    }

    int comp = strlen(chave);
    char *buf = malloc(sizeof(char) * (comp + 1));
    strcpy(buf, chave);

    bucket_no *novo_bucket = malloc(sizeof *novo_bucket);
    novo_bucket->chave = buf;
    novo_bucket->prox = NULL;
    novo_bucket->valor = valor;

    bucket_no *ptr = tabela->buckets[indice_chave];
    if (!ptr)
    {
        // cria um novo bucket apenas
        tabela->buckets[indice_chave] = novo_bucket;
    }
    else
    {
        // percorre até o final da lista e cria um novo bucket
        while (ptr->prox)
        {
            ptr = ptr->prox;
        }

        ptr->prox = novo_bucket;
    }

    tabela->itens++;
    return false;
}

bool tabela_hash_remove(tabela_hash *tabela, const char *chave)
{
    unsigned int hash_chave = hash(chave);
    unsigned int indice = hash_chave % tabela->comprimento;

    if (tabela->buckets[indice])
    {
        if (strcmp(tabela->buckets[indice]->chave, chave) == 0)
        {
            // remoção do primeiro elemento
            bucket_no *deletado = tabela->buckets[indice];
            tabela->buckets[indice] = deletado->prox;
            free(deletado->chave);
            free(deletado);
            return true;
        }
        else
        {
            // percorre o restante da tabela
            bucket_no *b = tabela->buckets[indice];
            while (b->prox && strcmp(b->prox->chave, chave) != 0)
            {
                b = b->prox;
            }
            if (b->prox)
            {
                bucket_no *deletado = b->prox;
                b->prox = deletado->prox;
                free(deletado->chave);
                free(deletado);
                return true;
            }
        }
    }

    return false;
}

tabela_hash *tabela_hash_copia(tabela_hash *tabela)
{
    tabela_hash *nova_tabela = tabela_hash_novo();

    for (size_t i = 0; i < tabela->comprimento; i++)
    {
        bucket_no *ptr = tabela->buckets[i];
        while (ptr)
        {
            tabela_hash_set(nova_tabela, ptr->chave, ptr->valor);
            ptr = ptr->prox;
        }
    }

    return nova_tabela;
}
