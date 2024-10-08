#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int string_compare_str(int *res, const String *a, const char *b) {
    String bs = {0};
    string_from_str(&bs, b);
    int r = string_compare(res, a, &bs);
    string_free(&bs);
    return r;
}

int string_equal(bool *res, const String *a, const String *b) {
    int cmp;
    string_compare(&cmp, a, b);
    *res = cmp == 0;
    return S_OK;
}

int string_equal_str(bool *res, const String *a, const char *b) {
    int cmp;
    string_compare_str(&cmp, a, b);
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

int parse_field_str(String *res, char **start, const char *sep) {
    char *end = strchr(*start, sep[0]);
    if (!end) {
        end = *start + strlen(*start);
    }
    char tmp = *end;
    *end = 0;
    int r = string_from_str(res, *start);
    *end = tmp;
    *start = end + 1;
    return r;
}

int parse_field_uint(unsigned *res, char **start, const char *sep) {
    char *ptr;
    *res = strtoul(*start, &ptr, 10);
    bool valid = *ptr == 0;
    for (; !valid && *sep; sep++) {
        if (*sep == *ptr) {
            valid = true;
        }
    }
    if (!valid) {
        ptr++;
        char tmp = *ptr;
        *ptr = 0;
        fprintf(stderr, "ERROR: failed to parse [%s] as unsigned\n", *start);
        *ptr = tmp;
        return S_PARSE_ERROR;
    }

    *start = ptr + 1;
    return S_OK;
}

int parse_field_float(float *res, char **start, const char *sep) {
    char *ptr;
    *res = strtof(*start, &ptr);
    bool valid = *ptr == 0;
    for (; !valid && *sep; sep++) {
        if (*sep == *ptr) {
            valid = true;
        }
    }
    if (!valid) {
        ptr++;
        char tmp = *ptr;
        *ptr = 0;
        fprintf(stderr, "ERROR: failed to parse [%s] as float\n", *start);
        *ptr = tmp;
        return S_PARSE_ERROR;
    }
    *start = ptr + 1;
    return S_OK;
}

int address_free(Adress *a) {
    string_free(&a->id);
    string_free(&a->city);
    string_free(&a->street);
    string_free(&a->block);
    return S_OK;
}

int mail_free(Mail *m) {
    string_free(&m->receive);
    string_free(&m->create);
    string_free(&m->id);
    address_free(&m->addr);
    return S_OK;
}

int address_from_string(Adress *a, char **s) {
    memset(a, 0, sizeof(Adress));
    check(parse_field_str(&a->city, s, ";"), address_free(a));
    check(parse_field_str(&a->street, s, ";"), address_free(a));
    check(parse_field_uint(&a->building, s, ";"), address_free(a));
    check(parse_field_str(&a->block, s, ";"), address_free(a));
    check(parse_field_uint(&a->flat, s, ";"), address_free(a));
    check(parse_field_str(&a->id, s, ";"), address_free(a));
    if (a->city.len == 0) {
        fprintf(stderr, "ERROR: city must not be empty\n");
        address_free(a);
        return S_PARSE_ERROR;
    }
    if (a->street.len == 0) {
        fprintf(stderr, "ERROR: street must not be empty\n");
        address_free(a);
        return S_PARSE_ERROR;
    }
    if (a->building == 0) {
        fprintf(stderr, "ERROR: building number must be positive\n");
        address_free(a);
        return S_PARSE_ERROR;
    }
    if (a->flat == 0) {
        fprintf(stderr, "ERROR: flat number must be positive\n");
        address_free(a);
        return S_PARSE_ERROR;
    }
    if (a->id.len != 6) {
        fprintf(stderr, "ERROR: mail index must be 6 symbols, got [%d] [%s]\n",
                a->id.len, a->id.ptr);
        address_free(a);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

int mail_from_string(Mail *m, char **s) {
    memset(m, 0, sizeof(Mail));
    check(address_from_string(&m->addr, s), mail_free(m));
    check(parse_field_float(&m->weight, s, ";"), mail_free(m));
    check(parse_field_str(&m->id, s, ";"), mail_free(m));
    check(parse_field_str(&m->create, s, ";"), mail_free(m));
    check(parse_field_str(&m->receive, s, ";"), mail_free(m));
    int read = 0;
    time_t t;
    sscanf(m->create.ptr, "%*2d:%*2d:%*4d %*2d:%*2d:%*2d%n", &read);
    if (read != 19 || parse_time(&t, &m->create) != S_OK) {
        fprintf(stderr, "ERROR: failed to parse [%s] as date\n", m->create.ptr);
        mail_free(m);
        return S_PARSE_ERROR;
    }
    sscanf(m->receive.ptr, "%*2d:%*2d:%*4d %*2d:%*2d:%*2d%n", &read);
    if (read != 19 || parse_time(&t, &m->receive) != S_OK) {
        fprintf(stderr, "ERROR: failed to parse [%s] as date\n",
                m->receive.ptr);
        mail_free(m);
        return S_PARSE_ERROR;
    }
    if (m->weight < 0) {
        mail_free(m);
        fprintf(stderr, "ERROR: weight must be >= 0\n");
        return S_PARSE_ERROR;
    }
    if (m->id.len != 14) {
        mail_free(m);
        fprintf(stderr, "ERROR: index must be 14 symbols, got [%d] [%s]\n",
                m->id.len, m->id.ptr);
        return S_PARSE_ERROR;
    }

    return S_OK;
}

int mail_print(const Mail *m) {
    printf("[%s %s %u %s %u %s] ", m->addr.city.ptr, m->addr.street.ptr,
           m->addr.building, m->addr.block.ptr, m->addr.flat, m->addr.id.ptr);
    printf("w=%f id=%s t_create=%s t_receive=%s\n", m->weight, m->id.ptr,
           m->create.ptr, m->receive.ptr);

    return S_OK;
}

int mail_cmp_id(const void *a0, const void *b0) {
    const Mail *a = a0;
    const Mail *b = b0;
    int res;
    string_compare(&res, &a->addr.id, &b->addr.id);
    if (res != 0)
        return res;
    string_compare(&res, &a->id, &b->id);
    return res;
}

int mail_cmp_date(const void *a0, const void *b0) {
    const Mail *a = a0;
    const Mail *b = b0;
    time_t ta;
    time_t tb;
    parse_time(&ta, &a->receive);
    parse_time(&tb, &b->receive);
    int diff = difftime(ta, tb);
    if (diff == 0)
        return 0;
    if (diff < 0)
        return -1;
    return 1;
}

int parse_time(time_t *t, const String *a) {
    struct tm t0 = {0};
    char *last = strptime(a->ptr, "%d:%m:%Y %H:%M:%S", &t0);
    if (last == NULL || *last != 0)
        return S_PARSE_ERROR;
    *t = mktime(&t0);
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
        printf("[%zu]:\t", i);
        mail_print(&p->mail[i]);
    }
    printf("\n");
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
    mail_free(&p->mail[i]);
    for (size_t j = i; j < p->size - 1; j++) {
        p->mail[j] = p->mail[j + 1];
    }
    p->size--;
    return S_OK;
}

int post_sort(Post *p) {
    qsort(p->mail, p->size, sizeof(Mail), mail_cmp_id);
    return S_OK;
}

int post_filter_by_delivery_status(Post *p, const Post *p0, bool delivered) {
    time_t now = time(NULL);
    time_t t;
    for (size_t i = 0; i < p0->size; i++) {
        Mail m = p0->mail[i];
        parse_time(&t, &m.receive);
        if ((difftime(t, now) <= 0) ^ !delivered) {
            post_add(p, m);
        }
    }
    qsort(p->mail, p->size, sizeof(Mail), mail_cmp_date);
    return S_OK;
}

int post_free(Post *p) {
    for (size_t i = 0; i < p->size; i++) {
        mail_free(&p->mail[i]);
    }
    free(p->mail);
    return S_OK;
}
