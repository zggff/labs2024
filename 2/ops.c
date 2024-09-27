#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ops.h"

int factorial(int n, ll *res) {
    if (n < 0)
        return MATH_INVALID_INPUT;
    if (n <= 1) {
        *res = 1;
        return MATH_OKAY;
    }
    ll new_res = 2;
    *res = new_res;
    for (int i = 3; i <= n; i++) {
        new_res *= i;
        if (new_res / i != *res) {
            return MATH_OVERFLOW;
        }
        *res = new_res;
    }
    return MATH_OKAY;
}

int factorial_d(int n, double *res) {
    if (n < 0)
        return MATH_INVALID_INPUT;
    if (n <= 1) {
        *res = 1;
        return MATH_OKAY;
    }
    *res = 2;
    for (int i = 3; i <= n; i++) {
        *res *= i;
        if (isinf(*res)) {
            return MATH_OVERFLOW;
        }
    }
    return MATH_OKAY;
}

int round_to_digits(int digits, double *res) {
    if (digits <= 0)
        return MATH_INVALID_INPUT;
    ll mul = pow(10, digits);
    ll res_ll = (ll)(*res * mul + 0.5);
    *res = (double)res_ll / mul;
    return MATH_OKAY;
}

int calculate_limit(double epsilon, calc op, double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));
    double prev;

    double n = 1;
    do {
        prev = *res;
        int status = op(n, res);
        if (status == MATH_OVERFLOW) {
            *res = prev;
            break;
        }
        if (status != MATH_OKAY) {
            return status;
        }
        n *= 2;
    } while (fabs(*res - prev) >= epsilon);
    return round_to_digits(digits, res);
}

int calculate_limit_based_on_prev(double epsilon, double start, calc op,
                                  double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));
    double prev;
    *res = start;
    do {
        prev = *res;
        int status = op(prev, res);
        if (status == MATH_OVERFLOW) {
            *res = prev;
            break;
        }
        if (status != MATH_OKAY) {
            return status;
        }
    } while (fabs(*res - prev) >= epsilon);
    return round_to_digits(digits, res);
}

int calculate_series(double epsilon, double start, calc op, double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));
    double diff = 0;
    *res = 0;
    double n = start;
    do {
        int status = op(n, &diff);
        if (status != MATH_OKAY)
            return status;
        *res += diff;
        n++;
    } while (fabs(diff) >= epsilon);
    return round_to_digits(digits, res);
}

int calculate_series_mul(double epsilon, double start, calc op, double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));
    double diff = 0;
    *res = 1;
    double prev = 1;
    double n = start;
    do {
        prev = *res;
        int status = op(n, &diff);
        if (status != MATH_OKAY)
            return status;
        *res *= diff;
        n++;
    } while (fabs(*res - prev) >= epsilon);
    return round_to_digits(digits, res);
}

int calculate_equation_binsearch(double epsilon, calc op, bool inc, double exp_res,
                                 double start, double end, double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));

    double upper = end;
    double lower = start;
    double op_res;
    while (fabs(upper - lower) >= epsilon) {
        *res = (upper + lower) / 2;
        int r = op(*res, &op_res);
        if (r != MATH_OKAY)
            return r;
        if ((op_res > exp_res) ^ (!inc)) {
            upper = *res;
        } else {
            lower = *res;
        }
    }
    return round_to_digits(digits, res);
}

int calculate_equation_linsearch(double epsilon, calc op, double exp_res,
                                 double start, double end, double *res) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    // double epsilon = pow(10.0, -(digits + 1));
    int digits = ceil(fabs(log10(epsilon)));

    double op_res;
    double min = 1000;
    for (double x = start; x <= end; x += epsilon) {
        int r = op(x, &op_res);
        if (r != MATH_OKAY) {
            return r;
        }
        if (fabs(op_res - exp_res) < min) {
            *res = x;
            min = fabs(op_res - exp_res);
        }
    }
    if (min < epsilon)
        return round_to_digits(digits, res);
    return MATH_INVALID_INPUT;
}
