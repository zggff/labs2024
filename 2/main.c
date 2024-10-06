#include <math.h>
#include <stdio.h>

#include <stdarg.h>

typedef enum RES {
    RES_OK = 0,
    RES_INCORRECT = 1,
} RES;

RES calc_average(double *res, int num, ...) {
    va_list valist;
    va_start(valist, num);
    *res = 1;
    int i = 0;
    for (; i < num; i++) {
        double arg = va_arg(valist, double);
        if (arg < 0) {
            va_end(valist);
            return RES_INCORRECT;
        }
        (*res) *= arg;
    }
    va_end(valist);
    *res = pow(*res, 1 / (double)num);
    return 0;
}

int handle_1(void) {
    double average;
    int res = calc_average(&average, 4, 0.1, 0.2, 0.4, 1.1);
    switch (res) {
    case 0:
        printf("%f\n", average);
        break;
    case 1:
        fprintf(stderr, "ERROR: arguments can only be positive\n");
        break;
    case 2:
        fprintf(stderr, "ERROR: invalid argument\n");
        break;
    }
    return 0;
}

int pow_custom(double *res, double x, int n) {
    if (n == 0) {
        *res = 1;
        return 0;
    }
    if (n == 1) {
        *res = x;
        return 0;
    }
    if (n < 0) {
        pow_custom(res, x, -n);
        *res = 1 / *res;
        return 0;
    }
    if (n % 2 == 1) {
        pow_custom(res, x, n - 1);
        *res *= x;
        return 0;
    }
    pow_custom(res, x, n / 2);
    *res = (*res) * (*res);
    return 0;
}

int handle_2(void) {
    double res = 1;
    double x = 0.5;
    int n = -11;
    pow_custom(&res, x, n);
    printf("%f^%d = %f\n", x, n, res);
    return 0;
}

int main(void) {
    handle_1();
    handle_2();
    return 0;
}
