#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Status {
    S_OK = 0,
    S_NOT_EMPTY = 1,
    S_MALLOC = 2,
} Status;

typedef struct String {
    int len;
    char *ptr;
} String;

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

int main(void) {
    String a = {0};
    String b = {0};
    String c = {0};
    string_from_str(&a, "hello, world!");
    string_from_str(&b, "\n");
    string_from_str(&c, "abcdefg");

    String d = {0};
    string_copy(&d, &a);
    printf("copy: [%s]\n", d.ptr);
    string_free(&d);

    String *e;
    string_create_copy(&e, &a);
    printf("copy into malloc: [%s]\n", e->ptr);
    string_free(e);
    free(e);

    string_concatenate(&a, &b);
    string_concatenate(&a, &c);
    printf("concatenated [%s]\n", a.ptr);

    string_free(&a);
    string_free(&b);
    string_free(&c);

    string_from_str(&a, "12345679");
    string_from_str(&b, "12345678");
    string_from_str(&c, "123456780");
    string_from_str(&d, "123456780");
    printf("\n");
    printf("a = [%s]\n", a.ptr);
    printf("b = [%s]\n", b.ptr);
    printf("c = [%s]\n", c.ptr);
    printf("d = [%s]\n", c.ptr);
    int res;
    string_compare(&res, &a, &b);
    printf("cmp(a, b) = %d\n", res);
    string_compare(&res, &a, &c);
    printf("cmp(a, c) = %d\n", res);
    string_compare(&res, &a, &a);
    printf("cmp(a, a) = %d\n", res);
    string_compare(&res, &c, &d);
    printf("cmp(c, d) = %d\n", res);
    bool eq;
    string_equal(&eq, &a, &b);
    printf("eq(a, b) = %d\n", eq);
    string_equal(&eq, &c, &d);
    printf("eq(c, d) = %d\n", eq);

    return 0;
}
