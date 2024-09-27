#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef long long ll;
typedef int (*calc)(double x, double n, double *res);

enum MATH_STATUS {
    MATH_OKAY = 0,
    MATH_INVALID_INPUT = 1,
    MATH_OVERFLOW = 2,
};

int factorial(int n, double *res) {
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

int factorial2(int n, double *res) {
    if (n < 0)
        return MATH_INVALID_INPUT;
    if (n <= 1) {
        *res = 1;
        return MATH_OKAY;
    }
    *res = 1;
    for (int i = n % 2 + 2; i <= n; i++) {
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

int calculate_series(double epsilon, double x, double *res, double start,
                     calc op) {
    if (epsilon <= 0 || res == NULL)
        return MATH_INVALID_INPUT;
    // n digits of precision => no changes in the n+1 digit
    int digits = ceil(fabs(log10(epsilon)));
    double diff = 0;
    *res = 0;
    int n = start;
    do {
        int status = op(x, n, &diff);
        if (status != MATH_OKAY)
            return status;
        *res += diff;
        n++;
    } while (fabs(diff) >= epsilon);
    return round_to_digits(digits, res);
}

int calc_a(double x, double n, double *res) {
    double fact;
    if (factorial(n, &fact) != MATH_OKAY)
        return MATH_OVERFLOW;
    *res = pow(x, n) / fact;
    return MATH_OKAY;
}

int calc_b(double x, double n, double *res) {
    double fact;
    if (factorial(2 * n, &fact) != MATH_OKAY)
        return MATH_OVERFLOW;
    *res = pow(-1, n) * pow(x, 2 * n) / fact;
    return MATH_OKAY;
}

int calc_c(double x, double n, double *res) {
    double fact;
    double fact3;
    if (factorial(n, &fact) != MATH_OKAY ||
        factorial(3 * n, &fact3) != MATH_OKAY)
        return MATH_OVERFLOW;
    *res = (pow(fact, 3) / fact3) * pow(3, 3 * n) * pow(x, 2 * n);
    if (isnan(*res))
        return MATH_OVERFLOW;

    return MATH_OKAY;
}

int calc_d(double x, double n, double *res) {
    double fact2m;
    double fact2;
    if (factorial2(2 * n - 1, &fact2m) != MATH_OKAY ||
        factorial2(2 * n, &fact2) != MATH_OKAY)
        return MATH_OVERFLOW;
    *res = (fact2m / fact2) * pow(-1, n) * pow(x, 2 * n);
    return MATH_OKAY;
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "ERROR: not enough args\n");
        return 1;
    }
    double epsilon;
    double x;
    if (sscanf(argv[1], "%lf", &epsilon) != 1 ||
        sscanf(argv[2], "%lf", &x) != 1) {
        fprintf(stderr, "ERROR: args must be numbers\n");
        return 1;
    }
    if (epsilon <= 0) {
        fprintf(stderr, "ERROR: epsilon must be greater than 0\n");
        return 1;
    }

    int digits = ceil(fabs(log10(epsilon)));

    int status;
    double res = 0;
    calc calcs[] = {calc_a, calc_b, calc_c, calc_d};
    int starts[] = {0, 0, 0, 1};

    for (int i = 0; i < (int)(sizeof(calcs) / sizeof(calc)); i++) {
        char c = 'a' + i;
        status = calculate_series(epsilon, x, &res, starts[i], calcs[i]);
        if (status) {
            fprintf(stderr, "%c)\tERROR: %d\n", c, status);
            continue;
        }
        printf("%c)\t%.*f\n", c, digits, res);
    }
}
