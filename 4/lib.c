#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

int string_from_str(String *s, const char *str) {
    if (s->len || s->ptr) {
        return S_NOT_EMPTY;
    }
    s->len = strlen(str);
    s->ptr = malloc(s->len + 1);
    if (!s->ptr)
        return S_MALLOC;
    memcpy(s->ptr, str, s->len + 1);
    return S_OK;
}

int string_free(String *s) {
    if (s->ptr)
        free(s->ptr);
    s->ptr = NULL;
    s->len = 0;
    return 0;
}

int string_compare(int *res, const String *a, const String *b) {
    if (a->len != b->len) {
        *res = a->len < b->len ? -1 : 1;
        return 0;
    }
    for (int i = 0; i < a->len; i++) {
        if (a->ptr[i] != b->ptr[i]) {
            *res = a->ptr[i] < b->ptr[i] ? -1 : 1;
            return 0;
        }
    }
    *res = 0;
    return S_OK;
}

int string_equal(bool *res, const String *a, const String *b) {
    int cmp;
    string_compare(&cmp, a, b);
    *res = cmp == 0;
    return S_OK;
}

int string_copy(String *a, const String *b) {
    if (a->len || a->ptr)
        string_free(a);
    return string_from_str(a, b->ptr);
}

int string_create_copy(String **a, const String *b) {
    *a = malloc(sizeof(String));
    if (*a == NULL)
        return S_MALLOC;
    return string_copy(*a, b);
}

int string_concatenate(String *a, const String *b) {
    if (a->len == 0)
        return string_copy(a, b);
    char *tmp = realloc(a->ptr, a->len + b->len + 1);
    if (tmp == NULL)
        return S_MALLOC;
    a->ptr = tmp;
    memcpy(a->ptr + a->len, b->ptr, b->len + 1);
    a->len = a->len + b->len;
    return S_OK;
}
