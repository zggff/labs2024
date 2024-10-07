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

int post_init(Post *p) {
    p->current = NULL;
    p->capacity = 16;
    p->size = 0;
    p->mail = malloc(p->capacity * sizeof(Mail));
    if (p->mail == NULL)
        return S_MALLOC;
    return S_OK;
}

int post_print(const Post *p) {
    for (size_t i = 0; i < p->size; i++) {
        mail_print(&p->mail[i]);
    }
    return S_OK;
}
int post_add(Post *p, Mail m) {
    if (p->size >= p->capacity) {
        size_t new_capacity = p->capacity * 2;
        Mail *tmp = realloc(p->mail, new_capacity);
        if (!tmp)
            return S_MALLOC;
        p->mail = tmp;
        p->capacity = new_capacity;
    }
    p->mail[p->size] = m;
    p->size++;
    return S_OK;
}
int post_remove(Post *p, size_t i) {
    if (i >= p->size)
        return S_OUT_OF_BOUNDS;
    for (size_t j = i; j < p->size - 1; j++) {
        p->mail[j] = p->mail[j + 1];
    }
    return S_OK;
}

int post_sort(Post *p) {
    qsort(p->mail, p->size, sizeof(Mail), mail_cmp);
    return S_OK;
}
