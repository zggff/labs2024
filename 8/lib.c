#include "lib.h"
#include <ctype.h>
#include <math.h>
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

int is_separator(int c) {
    return isspace(c) || c == '+' || c == '-' || c == ',' || c == ')' ||
           c == '\0';
}

int poly_parse(Poly *res, const char *str) {
    res->n = 0;
    const char *s = str;
    while (true) {
        char *end;
        s = strchr(s, '^');
        if (!s)
            break;
        s++;
        int pow = strtoul(s, &end, 10);
        if (*end != 0 && !is_separator(*end)) {
            int str_len = end - s;
            fprintf(stderr, "ERROR: failed to parse [%*s] as number\n", str_len,
                    s);
            return S_PARSE_ERROR;
        }
        if (pow > res->n)
            res->n = pow;
    }
    res->fs = malloc((res->n + 1) * sizeof(double));
    if (!res->fs)
        return S_MALLOC_ERROR;
    memset(res->fs, 0, (res->n + 1) * sizeof(double));

    s = str;
    while (*s) {
        while (isspace(*s))
            s++;
        bool neg = *s == '-';
        if (neg || *s == '+')
            s++;
        while (isspace(*s))
            s++;
        if (*s == 0)
            break;
        char *end;
        double coef = strtof(s, &end);
        if (*end != 'x' && !is_separator(*end)) {
            int str_len = end - s;
            fprintf(stderr, "ERROR: failed to parse [%*s] as float\n", str_len,
                    s);
            return S_PARSE_ERROR;
        }
        if (s == end)
            coef = 1;
        if (neg)
            coef *= -1;

        s = end;
        if (*s != 'x') { // a
            res->fs[0] = coef;
            continue;
        }
        s++;
        if (*s != '^') { // a * x
            res->fs[1] = coef;
            continue;
        }
        s++;
        int pow = strtoul(s, &end, 10);
        res->fs[pow] = coef;
        s = end;
    }

    return S_OK;
}

int poly_print(const Poly *a) {
    printf("%fx^%d", a->fs[a->n], a->n);
    bool printed = false;
    for (int n = a->n; n >= 2; n--) {
        printed = printed | (a->fs[n] != 0);
        if (a->fs[n] < 0)
            printf(" - %fx^%d", fabs(a->fs[n]), n);
        else if (a->fs[n] > 0)
            printf(" + %fx^%d", fabs(a->fs[n]), n);
    }
    if (a->n >= 1) {
        printed = printed | (a->fs[1] != 0);
        if (a->fs[1] < 0)
            printf(" - %fx", fabs(a->fs[1]));
        else if (a->fs[1] > 0)
            printf(" + %fx", fabs(a->fs[1]));
    }
    if (!printed)
        printf("%f", a->fs[0]);
    else {
        if (a->fs[0] < 0)
            printf(" - %f", fabs(a->fs[0]));
        else if (a->fs[0] > 0)
            printf(" + %f", fabs(a->fs[0]));
    }
    return 0;
}

#define SWAP_DESC(a, b)                                                        \
    {                                                                          \
        if (a->n > b->n) {                                                     \
            const Poly *_temp_val_ = a;                                        \
            a = b;                                                             \
            b = _temp_val_;                                                    \
        }                                                                      \
    }

int add_(double *res, double a, double b) {
    *res = a + b;
    return S_OK;
}
int sub_(double *res, double a, double b) {
    *res = a - b;
    return S_OK;
}

int poly_op_add_sub(Poly *res, const Poly *a, const Poly *b,
                    int (*op)(double *, double, double)) {
    SWAP_DESC(a, b);
    res->n = a->n;
    res->fs = malloc((res->n + 1) * sizeof(double));
    if (!res->fs)
        return S_MALLOC_ERROR;
    memset(res->fs, 0, (res->n + 1) * sizeof(double));
    int i;
    for (i = 0; i <= b->n; i++)
        op(&res->fs[i], a->fs[i], b->fs[i]);
    for (; i <= a->n; i++)
        res->fs[i] = a->fs[i];
    while (res->n > 0 && res->fs[res->n] == 0)
        res->n--;

    return S_OK;
}

inline int poly_add(Poly *res, const Poly *a, const Poly *b) {
    return poly_op_add_sub(res, a, b, add_);
}

inline int poly_sub(Poly *res, const Poly *a, const Poly *b) {
    return poly_op_add_sub(res, a, b, sub_);
}

int poly_div(Poly *res, const Poly *a, const Poly *b);
int poly_mod(Poly *res, const Poly *a, const Poly *b);
int poly_comp(Poly *res, const Poly *a, const Poly *b);
int poly_diff(Poly *res, const Poly *a);
int poly_eval(double *res, const Poly *a, double x);
