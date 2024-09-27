#ifndef __OPS_H__
#define __OPS_H__

typedef long long ll;
typedef int (*calc)(double n, double *res);

enum MATH_STATUS {
    MATH_OKAY = 0,
    MATH_INVALID_INPUT = 1,
    MATH_OVERFLOW = 2,
};

int factorial(int n, ll *res);
int factorial_d(int n, double *res);
int round_to_digits(int digits, double *res);
int calculate_limit(double epsilon, double *res, calc op);
int calculate_series(double epsilon, double *res, double start, calc op);
int calculate_equation_binsearch(double epsilon, double *res, calc op, double exp_res, double start, double end);
int calculate_equation_linsearch(double epsilon, double *res, calc op, double exp_res, double start, double end);

#endif
