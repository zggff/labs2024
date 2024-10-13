#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>


typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_PARSE_ERROR,
} Status;

typedef int (*mask)(int);
int parse_field_str(char **res, const char **start, mask m);
int parse_field_float(float *res, const char **start, mask m);
int parse_field_uint(unsigned long *res, const char **start, mask m);
int parse_field_char(char *res, char const **start, mask m);

typedef struct Polynomial {
    int n;
    double *fs; // array of n + 1 len
} Poly;

int poly_init(Poly *res, int n);
int poly_parse(Poly *res, const char *s);
int poly_free(Poly a);
int poly_print(Poly a);
int poly_mul_by_double(Poly *res, Poly a, double mul);
int poly_add(Poly *res, Poly a, Poly b);
int poly_sub(Poly *res, Poly a, Poly b);
int poly_mult(Poly *res, Poly a,Poly b);
int poly_div(Poly *res, Poly a, Poly b);
int poly_mod(Poly *res, Poly a, Poly b);
int poly_comp(Poly *res, Poly a, Poly b);
int poly_diff(Poly *res, Poly a);
int poly_eval(double *res, Poly a, double x);

int powd(double *res, double x, unsigned n);
int poly_pow(Poly *res, Poly p, unsigned n);

#endif
