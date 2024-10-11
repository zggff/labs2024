#include "lib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_field_str(char **res, char const **start, mask m) {
    const char *end = *start;
    while (*end && !m(*end)) {
        end++;
    }
    int n = end - *start;
    *res = malloc(n + 1);
    if (!res)
        return S_MALLOC_ERROR;
    memcpy(*res, *start, n);
    (*res)[n] = 0;
    *start = end + 1;
    return S_OK;
}

int parse_field_uint(unsigned long *res, char const **start, mask m) {
    char *ptr;
    *res = strtoul(*start, &ptr, 10);
    bool valid = *ptr == 0;
    valid = valid | m(*ptr);
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

int parse_field_float(float *res, char const **start, mask m) {
    char *ptr;
    *res = strtof(*start, &ptr);
    bool valid = *ptr == 0;
    valid = valid | m(*ptr);
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

int parse_field_time(tm *res, const char **start, mask m) {
    char *str;
    parse_field_str(&str, start, m);
    char *last = strptime(str, "%d.%m.%Y", res);
    if (last == NULL || *last != 0) {
        fprintf(stderr, "ERROR: failed to parse [%s] as date\n", str);
        free(str);
        return S_PARSE_ERROR;
    }
    res->tm_isdst = -1;
    free(str);
    return S_OK;
}

int parse_field_char(char *res, const char **start, mask m) {
    char *str;
    parse_field_str(&str, start, m);
    if (strlen(str) != 1) {
        fprintf(stderr, "ERROR: expected char, got string [%s]\n", str);
        free(str);
        return S_PARSE_ERROR;
    }
    *res = *str;
    free(str);
    return S_OK;
}

int validate_name(bool *res, const char *s) {
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (!isalpha(s[i])) {
            *res = false;
            return S_OK;
        }
    }
    *res = true;
    return S_OK;
}

int liver_from_str(Liver *l, const char *str) {
    const char *s = str;
    check(parse_field_str(&l->last_name, &s, isspace), liver_free(l));
    check(parse_field_str(&l->first_name, &s, isspace), liver_free(l));
    check(parse_field_str(&l->patronymic, &s, isspace), liver_free(l));
    check(parse_field_time(&l->date_of_birth, &s, isspace), liver_free(l));
    check(parse_field_char(&l->gender, &s, isspace), liver_free(l));
    check(parse_field_float(&l->income, &s, isspace), liver_free(l));

    bool valid;
    validate_name(&valid, l->last_name);
    if (!valid || l->last_name[0] == 0) {
        fprintf(stderr,
                "ERROR: last name must not be empty and can only "
                "contain latin symbols: [%s]\n",
                l->last_name);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    validate_name(&valid, l->first_name);
    if (!valid || l->first_name[0] == 0) {
        fprintf(stderr,
                "ERROR: first name must not be empty and can only "
                "contain latin symbols: [%s]\n",
                l->first_name);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    validate_name(&valid, l->patronymic);
    if (!valid) {
        fprintf(stderr,
                "ERROR: patronymic can only contain latin symbols: [%s]\n",
                l->patronymic);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    if (l->gender != 'M' && l->gender != 'W') {
        fprintf(stderr, "ERROR: gender can be either 'M' or 'W': [%c]\n",
                l->gender);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    if (l->income < 0) {
        fprintf(stderr, "ERROR: income must be positive: [%f]\n", l->income);
        liver_free(l);
        return S_PARSE_ERROR;
    }

    return S_OK;
}
int liver_free(Liver *l) {
    if (l->last_name)
        free(l->last_name);
    if (l->first_name)
        free(l->first_name);
    if (l->patronymic)
        free(l->patronymic);
    return S_OK;
}

int liver_print(const Liver *l) {
    return liver_write(l, stdout);
}

int liver_copy(Liver *t, const Liver *l) {
    int n;
    n = strlen(l->first_name) + 1;
    t->first_name = malloc(n);
    if (t == NULL)
        return S_MALLOC_ERROR;
    memcpy(t->first_name, l->first_name, n);

    n = strlen(l->last_name) + 1;
    t->last_name = malloc(n);
    if (t == NULL)
        return S_MALLOC_ERROR;
    memcpy(t->last_name, l->last_name, n);

    n = strlen(l->patronymic) + 1;
    t->patronymic = malloc(n);
    if (t == NULL)
        return S_MALLOC_ERROR;
    memcpy(t->patronymic, l->patronymic, n);

    t->date_of_birth = l->date_of_birth;
    t->gender = l->gender;
    t->income = l->income;
    return S_OK;
}

int liver_write(const Liver *l, FILE *f) {
    char t[20];
    strftime(t, 20, "%d.%m.%Y", &l->date_of_birth);
    fprintf(f, "%s %s %s %s %c %f\n", l->last_name, l->first_name,
            l->patronymic, t, l->gender, l->income);
    return S_OK;
}

int db_init(Db *d) {
    d->cap = 16;
    d->size = 0;
    d->ptr = malloc(d->cap * sizeof(Liver));
    if (d->ptr == NULL)
        return S_MALLOC_ERROR;

    d->op_cap = 16;
    d->op_size = 0;
    d->ops = malloc(d->op_cap * sizeof(Op));
    if (d->ptr == NULL) {
        free(d->ptr);
        return S_MALLOC_ERROR;
    }
    return S_OK;
}

int db_push_op(Db *d, Op op) {
    if (d->op_size >= d->op_cap) {
        size_t new_cap = d->op_cap * 2;
        Op *t = realloc(d->ops, new_cap * sizeof(Op));
        if (t == NULL)
            return S_MALLOC_ERROR;
        d->op_cap = new_cap;
        d->ops = t;
    }
    d->ops[d->op_size] = op;
    d->op_size++;
    return S_OK;
}

int db_pop_op(Db *d, Op *op) {
    if (d->size == 0)
        return S_UNDO_EMPTY_ERROR;
    d->op_size--;
    *op = d->ops[d->op_size];
    return S_OK;
}

int db_insert_(Db *d, size_t i, Liver l) {
    if (i > d->size)
        return S_OUT_OF_BOUNDS_ERROR;
    if (d->size >= d->cap) {
        size_t new_cap = d->cap * 2;
        Liver *t = realloc(d->ptr, new_cap * sizeof(Liver));
        if (t == NULL)
            return S_MALLOC_ERROR;
        d->cap = new_cap;
        d->ptr = t;
    }
    for (size_t j = d->size; j > i; j--)
        d->ptr[j] = d->ptr[j - 1];
    d->ptr[i] = l;
    d->size++;
    return S_OK;
}

int db_insert(Db *d, size_t i, Liver l) {
    Op op = {.type = OP_INSERT, .pos = i, .l = l};
    check(db_insert_(d, i, l), {});
    check(db_push_op(d, op), {});
    return S_OK;
}

int db_push_(Db *d, Liver l) {
    return db_insert_(d, d->size, l);
}

int db_push(Db *d, Liver l) {
    return db_insert(d, d->size, l);
}

int db_remove_(Db *d, size_t i) {
    if (i >= d->size)
        return S_OUT_OF_BOUNDS_ERROR;
    for (size_t j = i; j < d->size - 1; j++) {
        d->ptr[j] = d->ptr[j + 1];
    }
    d->size--;
    return S_OK;
}

int db_remove(Db *d, size_t i) {
    if (i >= d->size)
        return S_OUT_OF_BOUNDS_ERROR;
    Op op = {.type = OP_REMOVE, .pos = i, .l = d->ptr[i]};
    check(db_remove_(d, i), {});
    check(db_push_op(d, op), {});
    return S_OK;
}

int db_update_(Db *d, size_t i, Liver l) {
    if (i >= d->size)
        return S_OUT_OF_BOUNDS_ERROR;
    d->ptr[i] = l;
    return S_OK;
}

int db_update(Db *d, size_t i, Liver l) {
    if (i >= d->size)
        return S_OUT_OF_BOUNDS_ERROR;
    Liver l_old = {0};
    liver_copy(&l_old, &d->ptr[i]);
    Op op = {.type = OP_UPDATE, .pos = i, .l = l_old};
    liver_free(&d->ptr[i]);
    check(db_update_(d, i, l), {});
    check(db_push_op(d, op), {});
    return S_OK;
}

int db_undo(Db *d) {
    Op op;
    check(db_pop_op(d, &op),
          fprintf(stderr,
                  "ERROR: impossible to undo, not actions were performed"));
    switch (op.type) {
    case OP_INSERT:
        check(db_remove_(d, op.pos), {});
        liver_free(&op.l);
        break;
    case OP_UPDATE:
        liver_free(&d->ptr[op.pos]);
        check(db_update_(d, op.pos, op.l), {});
        break;
    case OP_REMOVE:
        check(db_insert_(d, op.pos, op.l), {});
        break;
    default:
        fprintf(stderr, "ERROR: unreachable\n");
    }
    return S_OK;
}

int db_read_file(Db *d, FILE *f) {
    char *line = NULL;
    size_t line_len = 0;
    while (true) {
        int n = getline(&line, &line_len, f);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;
        const char *s = line;
        Liver l = {0};
        check(liver_from_str(&l, s), free(line));
        check(db_push_(d, l), free(line));
    }
    free(line);
    return S_OK;
}

int db_free(Db *d) {
    for (size_t i = 0; i < d->size; i++) {
        liver_free(&d->ptr[i]);
    }
    for (size_t i = 0; i < d->op_size; i++) {
        liver_free(&d->ops[i].l);
    }
    free(d->ptr);
    free(d->ops);
    return S_OK;
}
int db_write(const Db *d, FILE *f) {
    for (size_t i = 0; i < d->size; i++) {
        liver_write(&d->ptr[i], f);
    }
    return S_OK;
}
int db_print(const Db *d) {
    for (size_t i = 0; i < d->size; i++) {
        printf("[%zu]:\t", i);
        liver_print(&d->ptr[i]);
    }
    printf("\n");
    return S_OK;
}
