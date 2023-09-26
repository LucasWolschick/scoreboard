#include "vector.h"

/* vector_new() inicializa um vetor vazio, o qual possuí capacidade, tamanho e
 * um ponteiro (utilizado como índice). A função retorna o vetor inicializado.
 *
 *
 */
vector vector_new(void)
{
    vector v;
    v.capacity = 0;
    v.len = 0;
    v.ptr = 0;
    return v;
}

/* vector_destroy() libera a memória alocada para o vetor passado como parâmetro.
 */
void vector_destroy(vector *v)
{
    free(v->ptr);
    v->ptr = 0;
    v->capacity = 0;
    v->len = 0;
}

/* vector_expand() expande o vetor passado como parâmetro, dobrando a capacidade
 * atual.
 */
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
    v->ptr = realloc(v->ptr, sizeof(VECTOR_TYPE) * (v->capacity));
}

/* vector_push() adiciona um elemento ao vetor passado como parâmetro. Caso o vetor
 * esteja cheio, a função vector_expand() é chamada para dobrar a capacidade do vetor.
 */
void vector_push(vector *v, VECTOR_TYPE elem)
{
    if (v->len + 1 > v->capacity)
    {
        vector_expand(v);
    }
    v->ptr[v->len] = elem;
    v->len++;
}

/* vector_pop() remove um elemento do vetor passado como parâmetro. Caso o vetor
 * não esteja vazio, o elemento é removido e a função retorna true, caso contrário
 * a função retorna false.
 */
bool vector_pop(vector *v, VECTOR_TYPE *elem)
{
    if (v->len > 0)
    {
        VECTOR_TYPE e = v->ptr[v->len];
        v->len--;
        *elem = e;
        return true;
    }
    return false;
}

/* vector_get() retorna o elemento do vetor passado como parâmetro no índice i.
 * Caso o índice seja inválido, a função retorna false, caso contrário a função
 * retorna true e o elemento é retornado por referência.
 */
bool vector_get(vector *v, size_t i, VECTOR_TYPE *elem)
{
    if (0 <= i && i < v->len)
    {
        *elem = v->ptr[i];
        return true;
    }
    return false;
}

/* vector_set() escreve o elemento elem no vetor passado como parâmetro no índice i.
 * Caso o índice seja inválido, a função retorna false, caso contrário a função
 * retorna true.
 */
bool vector_set(vector *v, size_t i, VECTOR_TYPE elem)
{
    if (0 <= i && i < v->len)
    {
        v->ptr[i] = elem;
        return true;
    }
    return false;
}

/* vector_len() retorna o tamanho do vetor passado como parâmetro. */
size_t vector_len(vector *v)
{
    return v->len;
}