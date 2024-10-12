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
    res->n = strchr(str, 'x') != 0;
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

int poly_print(Poly a) {
    for (int n = a.n; n >= 0; n--) {
        char op = a.fs[n] < 0 ? '-' : '+';
        char *x;
        if (n == 0)
            x = "";
        else if (n == 1)
            x = "x";
        else
            x = "x^%d";
        if (n == a.n)
            printf("%f", a.fs[n]);
        else
            printf("%c %f", op, fabs(a.fs[n]));
        printf(x, n);
        if (n > 0)
            printf(" ");
    }
    return 0;
}

#define SWAP_DESC(a, b)                                                        \
    {                                                                          \
        if (a.n > b.n) {                                                       \
            const Poly _temp_val_ = a;                                         \
            a = b;                                                             \
            b = _temp_val_;                                                    \
        }                                                                      \
    }

int poly_init(Poly *res, int n) {
    res->n = n;
    res->fs = malloc((res->n + 1) * sizeof(double));
    if (!res->fs)
        return S_MALLOC_ERROR;
    memset(res->fs, 0, (res->n + 1) * sizeof(double));
    return S_OK;
}

int poly_free(Poly a) {
    free(a.fs);
    return S_OK;
}

int poly_mul_by_double(Poly *res, Poly a, double mul) {
    poly_init(res, a.n);
    for (int i = 0; i <= res->n; i++)
        res->fs[i] = a.fs[i] * mul;
    return S_OK;
}

int poly_op_add(Poly *res, Poly a, Poly b) {
    int r, i;
    SWAP_DESC(a, b);
    if ((r = poly_init(res, a.n)))
        return r;
    for (i = 0; i <= b.n; i++)
        res->fs[i] = a.fs[i] + b.fs[i];
    for (; i <= a.n; i++)
        res->fs[i] = a.fs[i];
    while (res->n > 0 && res->fs[res->n] == 0)
        res->n--;

    return S_OK;
}

int poly_sub(Poly *res, Poly a, Poly b) {
    Poly neg;
    int r;
    if ((r = poly_mul_by_double(&neg, b, -1)))
        return r;
    r = poly_op_add(res, a, neg);
    poly_free(neg);
    return r;
}

int poly_mult(Poly *res, Poly a, Poly b) {
    SWAP_DESC(a, b);
    int r;
    if ((r = poly_init(res, a.n + b.n)))
        return r;
    for (int i = 0; i <= a.n; i++) {
        for (int j = 0; j <= b.n; j++) {
            res->fs[i + j] += a.fs[i] * b.fs[j];
        }
    }
    return S_OK;
}

int powd(double *res, double x, unsigned exp) {
    *res = 1;
    while (exp) {
        if (exp & 1)
            (*res) *= x;
        exp >>= 1;
        x *= x;
    }
    return 0;
}

int poly_pow(Poly *res, Poly p, unsigned exp) {
    Poly result, base;
    if (poly_init(&result, 0) || poly_mul_by_double(&base, p, 1))
        return S_MALLOC_ERROR;
    result.fs[0] = 1;
    while (exp) {
        if (exp & 1) {
            Poly new_r;
            if (poly_mult(&new_r, result, base)) {
                poly_free(result);
                poly_free(base);
                return S_MALLOC_ERROR;
            }
            poly_free(result);
            result = new_r;
        }
        exp >>= 1;
        Poly new_base;
        if (poly_mult(&new_base, base, base)) {
            poly_free(base);
            poly_free(result);
            return S_MALLOC_ERROR;
        }
        poly_free(base);
        base = new_base;
    }
    poly_free(base);
    *res = result;
    return S_OK;
}
