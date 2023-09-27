#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define VECTOR_TYPE uint32_t


/*
 * Struct que contém um vetor dinâmico.
 */
typedef struct vector
{
    size_t capacity;
    size_t len;
    VECTOR_TYPE *ptr;
} vector;

vector vector_new(void);

void vector_destroy(vector *v);

void vector_expand(vector *v);

void vector_push(vector *v, VECTOR_TYPE elem);

bool vector_pop(vector *v, VECTOR_TYPE *elem);

bool vector_get(vector *v, size_t i, VECTOR_TYPE *elem);

bool vector_set(vector *v, size_t i, VECTOR_TYPE elem);

size_t vector_len(vector *v);

#endif