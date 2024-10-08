#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "lib.h"


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

int validate_string_only_latin(bool *res, const char *s) {
    if (*s == 0) {
        *res = false;
        return 0;
    }
    for (; *s; s++) {
        if (!isalpha(*s)) {
            *res = false;
            return 0;
        }
    }
    *res = true;
    return 0;
}

int validate_string(bool *res, const char *s) {
    *res = *s != 0;
    return 0;
}

#define check(MACRO)                                                           \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            student_free(s);                                                   \
            return r;                                                          \
        }                                                                      \
    }

#define VALIDATE(VALIDATOR, FIELD)                                             \
    {                                                                          \
        bool valid;                                                            \
        VALIDATOR(&valid, s->FIELD);                                           \
        if (!valid) {                                                          \
            fprintf(stderr, "ERROR: invalid %s: \"%s\"\n", #FIELD, s->FIELD);  \
            student_free(s);                                                   \
            return S_PARSE_ERROR;                                              \
        }                                                                      \
    }

int student_from_str(Student *s, const char *str) {
    unsigned long n;
    s->grades = malloc(5);
    if (s->grades == NULL) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return S_MALLOC_ERROR;
    }
    const char *str2 = str;
    check(parse_field_uint(&s->id, &str2, isspace));
    check(parse_field_str(&s->first_name, &str2, isspace));
    VALIDATE(validate_string_only_latin, first_name);
    check(parse_field_str(&s->last_name, &str2, isspace));
    VALIDATE(validate_string_only_latin, last_name);
    check(parse_field_str(&s->group, &str2, isspace));
    VALIDATE(validate_string, group);

    for (int i = 0; i < 5; i++) {
        check(parse_field_uint(&n, &str2, isspace));
        if (n > UCHAR_MAX) {
            student_free(s);
            fprintf(stderr, "ERROR: invalid grade: \"%lu\"\n", n);
            return S_PARSE_ERROR;
        }
        s->grades[i] = n;
    }
    return S_OK;
}

int student_free(Student *s) {
    if (s->group)
        free(s->group);
    if (s->first_name)
        free(s->first_name);
    if (s->last_name)
        free(s->last_name);
    if (s->grades)
        free(s->grades);
    return S_OK;
}
int student_print(const Student *s) {
    printf("%lu %s %s %s [", s->id, s->first_name, s->last_name, s->group);
    for (int i = 0; i < 4; i++) {
        printf("%d, ", s->grades[i]);
    }
    printf("%d]", s->grades[4]);
    return S_OK;
}

#define CMP_MACRO_UNWRAP(TYPE)                                                 \
    const TYPE *a = a0;                                                        \
    const TYPE *b = b0

#define CMP_MACRO(TYPE, FIELD)                                                 \
    CMP_MACRO_UNWRAP(TYPE);                                                    \
    return strcmp(a->FIELD, b->FIELD)

int cmp_student_id(const void *a0, const void *b0) {
    CMP_MACRO_UNWRAP(Student);
    if (a->id < b->id)
        return -1;
    return a->id == b->id ? 0 : 1;
}
int cmp_student_ln(const void *a0, const void *b0) {
    CMP_MACRO(Student, last_name);
}
int cmp_student_fn(const void *a0, const void *b0) {
    CMP_MACRO(Student, first_name);
}
int cmp_student_gr(const void *a0, const void *b0) {
    CMP_MACRO(Student, group);
}

int db_init(Db *d) {
    d->size = 0;
    d->cap = 16;
    d->not_original = false;
    d->ptr = malloc(d->cap * sizeof(Student));
    if (d->ptr == NULL)
        return S_MALLOC_ERROR;
    d->avg_grade = 0;
    return S_OK;
}
int db_free(Db *d) {
    if (!d->not_original) {
        for (size_t i = 0; i < d->size; i++) {
            student_free(&d->ptr[i]);
        }
    }
    free(d->ptr);
    return S_OK;
}
int db_print(const Db *d) {
    printf("Number of students: %zu, average grade: %f\n", d->size, d->avg_grade);
    for (size_t i = 0; i < d->size; i++) {
        printf("[%zu]\t", i);
        student_print(&d->ptr[i]);
        printf("\n");
    }
    return S_OK;
}
int db_search_id(Db *res, const Db *d, unsigned long id) {
    res->not_original = true;
    for (size_t i = 0; i < d->size; i++) {
        Student s = d->ptr[i];
        if (s.id == id)
            db_push(res, s);
    }
    return S_OK;
}
int db_search_ln(Db *res, const Db *d, const char *ln) {
    res->not_original = true;
    for (size_t i = 0; i < d->size; i++) {
        Student s = d->ptr[i];
        if (strcmp(s.last_name, ln) == 0)
            db_push(res, s);
    }
    return S_OK;
}
int db_search_fn(Db *res, const Db *d, const char *fn) {
    res->not_original = true;
    for (size_t i = 0; i < d->size; i++) {
        Student s = d->ptr[i];
        if (strcmp(s.first_name, fn) == 0)
            db_push(res, s);
    }
    return S_OK;
}
int db_search_gr(Db *res, const Db *d, const char *gr) {
    res->not_original = true;
    for (size_t i = 0; i < d->size; i++) {
        Student s = d->ptr[i];
        if (strcmp(s.group, gr) == 0)
            db_push(res, s);
    }
    return S_OK;
}

int db_sort_id(const Db *d) {
    qsort(d->ptr, d->size, sizeof(Student), cmp_student_id);
    return S_OK;
}
int db_sort_ln(const Db *d) {
    qsort(d->ptr, d->size, sizeof(Student), cmp_student_ln);
    return S_OK;
}
int db_sort_fn(const Db *d) {
    qsort(d->ptr, d->size, sizeof(Student), cmp_student_fn);
    return S_OK;
}
int db_sort_gr(const Db *d) {
    qsort(d->ptr, d->size, sizeof(Student), cmp_student_gr);
    return S_OK;
}
int db_from_file(Db *d, FILE *f) {
    size_t line_len = 0;
    char *line = NULL;
    while (true) {
        int n = getline(&line, &line_len, f);
        if (n <= 1)
            break;
        n--;
        line[n] = 0;
        Student s = {0};
        int r = student_from_str(&s, line);
        if (r) {
            fprintf(stderr, "ERROR: failed to parse [%s] as student\n", line);
            return r;
        }
        r = db_push(d, s);
        if (r)
            return r;
    }
    if (line)
        free(line);
    return S_OK;
}
int db_push(Db *d, Student s) {
    if (d->size >= d->cap) {
        size_t new_cap = d->cap * 2;
        Student *tmp = realloc(d->ptr, new_cap * sizeof(Student));
        if (!tmp)
            return S_MALLOC_ERROR;
        d->cap = new_cap;
        d->ptr = tmp;
    }
    d->ptr[d->size] = s;
    float total = d->avg_grade * (d->size) * 5;
    d->size++;
    for (int i = 0; i < 5; i++)
        total += s.grades[i];
    d->avg_grade = total / (d->size * 5);
    return S_OK;
}
