#include <math.h>
#include <stdio.h>
#include "ops.h"

int e_series(double n, double *res) {
    ll fact = 1;
    if (factorial(n, &fact) != MATH_OKAY)
        return MATH_OVERFLOW;
    *res = 1.0 / fact;
    return MATH_OKAY;
}

int e_limit(double n, double *res) {
    *res = pow(1.0 + 1.0 / n, n);
    return MATH_OKAY;
}

int e_equation(double n, double *res) {
    *res = log(n);
    return MATH_OKAY;
}

int pi_limit(double n, double *res) {
    // невозможно посчитать из-за ограничений long long
    double n_fac;
    double n2_fac;
    if (factorial_d(n, &n_fac) != MATH_OKAY ||
        factorial_d(2 * n, &n2_fac) != MATH_OKAY) {
        return MATH_OVERFLOW;
    }
    *res = pow((n_fac * n_fac / n2_fac * pow(2, n*2)), 2) / n;
    return MATH_OKAY;
}

int pi_series(double n, double *res) {
    *res = 4 * pow(-1, n - 1) / (2 * n - 1);
    return MATH_OKAY;
}

int pi_equation(double n, double *res) {
    *res = cos(n);
    // невозможно посчитать из-за непостоянности функции
    return MATH_OKAY;
}

int ln_limit(double n, double *res) {
    *res = n * (pow(2, 1 / n) - 1);
    return MATH_OKAY;
}

int ln_series(double n, double *res) {
    *res = pow(-1, n - 1) / n;
    return MATH_OKAY;
}

int ln_equation(double n, double *res) {
    *res = exp(n);
    return MATH_OKAY;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "ERROR: no epsilon provided\n");
        return 1;
    }
    double epsilon;
    if (sscanf(argv[1], "%lf", &epsilon) != 1) {
        fprintf(stderr, "ERROR: epsilon must be a number\n");
        return 1;
    }
    if (epsilon <= 0) {
        fprintf(stderr, "ERROR: epsilon must be greater than 0\n");
        return 1;
    }
    int digits = ceil(fabs(log10(epsilon)));

    double e = 0;
    if (calculate_limit(epsilon, &e, e_limit) == MATH_OKAY)
        printf("limit\te=%.*f\n", digits, e);
    if (calculate_series(epsilon, &e, 0, e_series) == MATH_OKAY)
        printf("series\te=%.*f\n", digits, e);
    if (calculate_equation_binsearch(epsilon, &e, e_equation, 1, 2, 4) ==
        MATH_OKAY)
        printf("equati\te=%.*f\n", digits, e);

    double pi = 0;
    printf("\n");
    if (calculate_limit(epsilon, &pi, pi_limit) == MATH_OKAY)
        printf("limit\tpi=%.*f\n", digits, pi);
    if (calculate_series(epsilon, &pi, 1, pi_series) == MATH_OKAY)
        printf("series\tpi=%.*f\n", digits, pi);
    if (calculate_equation_linsearch(epsilon, &pi, pi_equation, -1, 3, 4) ==
        MATH_OKAY)
        printf("equati\tpi=%.*f\n", digits, pi);

    double ln = 0;
    printf("\n");
    if (calculate_limit(epsilon, &ln, ln_limit) == MATH_OKAY)
        printf("limit\tln=%.*f\n", digits, ln);
    if (calculate_series(epsilon, &ln, 1, ln_series) == MATH_OKAY)
        printf("series\tln=%.*f\n", digits, ln);
    if (calculate_equation_binsearch(epsilon, &ln, ln_equation, 2, 0, 1) ==
        MATH_OKAY)
        printf("equati\tln=%.*f\n", digits, ln);

    return 0;
}
