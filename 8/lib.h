#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int _r = MACRO;                                                        \
        if (_r) {                                                              \
            AFTER;                                                             \
            return _r;                                                         \
        }                                                                      \
    }

typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_PARSE_ERROR,
} Status;

typedef int (*mask)(int);
int parse_field_str(char **res, char const **start, mask m);
int parse_field_float(float *res, char const **start, mask m);
int parse_field_uint(unsigned long *res, char const **start, mask m);
int parse_field_char(char *res, char const **start, mask m);

typedef struct Polynomial {
    int n;
    double *fs; // array of n + 1 len
} Poly;

int poly_parse(Poly *res, const char *s);
int poly_print(const Poly *a);
int poly_add(Poly *res, const Poly *a, const Poly *b);
int poly_add(Poly *res, const Poly *a, const Poly *b);
int poly_sub(Poly *res, const Poly *a, const Poly *b);
int poly_div(Poly *res, const Poly *a, const Poly *b);
int poly_mod(Poly *res, const Poly *a, const Poly *b);
int poly_comp(Poly *res, const Poly *a, const Poly *b);
int poly_diff(Poly *res, const Poly *a);
int poly_eval(double *res, const Poly *a, double x);

#endif
