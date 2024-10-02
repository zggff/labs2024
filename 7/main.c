
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PRODUCT(A, B) ((A) * (B))
#define SUM(A, B) ((A) + (B))
#define JUST_A_FUNCTION(A, B, MACRO) MACRO(A, B)

#define check_and_return(MACRO)                                                \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            return r;                                                          \
        }                                                                      \
    }

typedef int (*calc)(double *y, double x);

typedef enum Status {
    S_OK,
    S_INVALID_INPUT,
    S_NOT_MONOTONE,
    S_NOT_IN_RANGE
} Status;

Status solve(double *res, double x0, double x1, double epsilon, calc op,
             double exp_res) {
    if (epsilon <= 0 || res == NULL || x0 >= x1)
        return S_INVALID_INPUT;

    double y0, y1;
    check_and_return(op(&y0, x0));
    check_and_return(op(&y1, x1));
    bool inc = y0 < y1;
    if ((exp_res > y0 && exp_res > y1) || (exp_res < y0 && exp_res < y1))
        return S_NOT_IN_RANGE;

    int steps = ceil(log2((x1 - x0) / epsilon));

    while (fabs(x1 - x0) >= epsilon && steps > 0) {
        steps--;
        *res = (x0 + x1) / 2;
        check_and_return(op(&y0, *res));
        if ((y0 > exp_res) ^ (inc)) {
            x0 = *res;
        } else {
            x1 = *res;
        }
    }
    if (fabs(x1 - x0) > epsilon)
        return S_NOT_MONOTONE;

    return S_OK;
}

int sqrx(double *y, double x) {
    *y = x * x;
    return 0;
}

int sinx(double *y, double x) {
    *y = sin(x);
    return 0;
}

int solve_with_notes(double x0, double x1, double epsilon, calc op,
                     double exp_res) {
    double res;
    int status = solve(&res, x0, x1, epsilon, op, exp_res);
    switch (status) {
    case S_OK:
        printf("%f\n", res);
        break;
    case S_INVALID_INPUT:
        fprintf(
            stderr,
            "ERROR: invalid input: epsilon <= 0 or result pointer is null\n");
        break;
    case S_NOT_IN_RANGE:
        fprintf(stderr, "ERROR: right side value is not in range of function "
                        "on given range of x\n");
        break;
    case S_NOT_MONOTONE:
        fprintf(stderr, "ERROR: function is not monotone\n");
        break;
    }
    return status;
}

int main(void) {
    solve_with_notes(2, 4, 0.0001, sinx, 0);
    solve_with_notes(0, 2, 0.0001, sqrx, 2);
}
