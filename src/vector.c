#include "vector.h"

vector vector_new(void)
{
    vector v;
    v.capacity = 0;
    v.len = 0;
    v.ptr = 0;
    return v;
}

void vector_destroy(vector *v)
{
    free(v->ptr);
    v->ptr = 0;
    v->capacity = 0;
    v->len = 0;
}

void vector_expand(vector *v)
{
    if (v->capacity < 2)
    {
        v->capacity = 2;
    }
    else
    {
        v->capacity *= 2;
    }
    v->ptr = realloc(v->ptr, sizeof(int) * (v->capacity));
}

void vector_push(vector *v, int elem)
{
    if (v->len + 1 > v->capacity)
    {
        vector_expand(v);
    }
    v->ptr[v->len] = elem;
    v->len++;
}

bool vector_pop(vector *v, int *elem)
{
    if (v->len > 0)
    {
        int e = v->ptr[v->len];
        v->len--;
        *elem = e;
        return true;
    }
    return false;
}

bool vector_get(vector *v, size_t i, int *elem)
{
    if (0 <= i && i < v->len)
    {
        *elem = v->ptr[i];
        return true;
    }
    return false;
}

bool vector_set(vector *v, size_t i, int elem)
{
    if (0 <= i && i < v->len)
    {
        v->ptr[i] = elem;
        return true;
    }
    return false;
}

size_t vector_len(vector *v)
{
    return v->len;
}