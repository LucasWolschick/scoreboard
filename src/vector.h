#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct vector
{
    size_t capacity;
    size_t len;
    int *ptr;
} vector;

vector vector_new(void);

void vector_destroy(vector *v);

void vector_expand(vector *v);

void vector_push(vector *v, int elem);

bool vector_pop(vector *v, int *elem);

bool vector_get(vector *v, size_t i, int *elem);

bool vector_set(vector *v, size_t i, int elem);

size_t vector_len(vector *v);

#endif