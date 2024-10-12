
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            AFTER;                                                             \
            return r;                                                          \
        }                                                                      \
    }

typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_INVALID_INPUT_ERROR,
} Status;

int binomial(double *res, int n, int k) {
    if (n < 0 || k < 0 || k > n) {
        return S_INVALID_INPUT_ERROR;
    }
    *res = 1;
    for (int i = n - k + 1; i <= n; i++) {
        *res *= i;
    }
    for (int i = 2; i <= k; i++) {
        *res /= i;
    }
    return S_OK;
}

int pow_custom(double *res, double x, int n) {
    bool rev = n < 0;
    if (rev)
        n *= -1;
    *res = 1;
    for (;;) {
        if (n & 1)
            (*res) *= x;
        n >>= 1;
        if (!n)
            break;
        x *= x;
    }
    if (rev)
        *res = 1 / *res;
    return 0;
}

int calculate_binomial(double a, int n, double f, double *res) {
    // calculate (x - a)^n
    for (int k = 0; k <= n; k++) {
        double bin;
        binomial(&bin, n, k);
        double ap;
        pow_custom(&ap, a, k);
        bin *= ap;
        // bin *= pow(a, k);
        bin *= f;
        res[n - k] += bin;
    }
    return S_OK;
}

int convert(double epsilon, double a, double **res, int n, ...) {
    (void)epsilon;
    double *fs = malloc((n + 1) * sizeof(double));
    if (!fs) {
        return S_MALLOC_ERROR;
    }
    va_list valist;
    va_start(valist, n);
    for (int i = 0; i <= n; i++) {
        fs[i] = va_arg(valist, double);
    }
    va_end(valist);

    double *gs = malloc((n + 1) * sizeof(double));
    if (!gs) {
        free(fs);
        return S_MALLOC_ERROR;
    }
    memset(gs, 0, n * sizeof(double));
    for (int i = n; i >= 0; i--) {
        calculate_binomial(a, i, fs[i], gs);
    }
    free(fs);
    *res = gs;
    return S_OK;
}

int main(void) {
    double *res;
    int n;

    n = 5;
    check(convert(0.001, 0.9, &res, n, 2.4, 0.5, -0.2, 12.4, -8.2, 12.1), {});
    for (int i = 0; i <= n; i++) {
        printf("%f ", res[i]);
    }
    printf("\n");
    free(res);

    n = 4;
    check(convert(0.001, 3, &res, n, -2.0, 1.0, -3.0, 0.0, 1.0), {});
    for (int i = 0; i <= n; i++) {
        printf("%f ", res[i]);
    }
    printf("\n");
    free(res);

    return 0;
}
