
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            AFTER;                                                             \
            return r;                                                          \
        }                                                                      \
    }

#define EPSILON 0.000000000001

typedef enum Status {
    S_OK,
    S_INVALID_INPUT_ERROR,
    S_INVALID_BASE_ERROR,
    S_MALLOC_ERROR,
} Status;

int check_fraction(bool *fin, int base, double x, double e) {
    *fin = false;
    int cap = 128;
    double *rem = malloc(cap * sizeof(double));
    if (!rem)
        return S_MALLOC_ERROR;

    int i = 0;
    while (true) {
        x *= base;
        x -= (int)x;
        if ((int)ceil(x) == 0) {
            *fin = true;
            break;
        }
        bool end = false;
        for (int j = 0; j < i; j++) {
            if (fabs(x - rem[j]) < e) {
                end = true;
                break;
            }
        }
        if (end)
            break;

        if (i >= cap) {
            break;
            int new_cap = cap * 2;
            double *t = realloc(rem, new_cap * sizeof(double));
            if (!t) {
                free(rem);
                return S_MALLOC_ERROR;
            }
            rem = t;
            cap = new_cap;
        }
        rem[i] = x;
        i++;
    }
    free(rem);
    return S_OK;
}

int check_fractions(int base, ...) {
    if (base < 2)
        return S_INVALID_BASE_ERROR;

    va_list valist;
    va_start(valist, base);
    while (true) {
        double x = va_arg(valist, double);
        if (x <= 0 || x >= 1)
            break;
        bool fin;
        check(check_fraction(&fin, base, x, EPSILON), va_end(valist));
        if (fin)
            printf("%f is a finite fraction in base %d\n", x, base);
        else
            printf("%f is not a finite fraction in base %d\n", x, base);
    }

    va_end(valist);
    return S_OK;
}

int main(void) {
    check_fractions(3, 0.1, 1.0 / 3.0, 0.33333, 0.5, 0.0, -1.0);
    printf("\n");
    check_fractions(2, 0.1, 0.2, 0.5, 1.0 / 1024, -1.0);
    printf("\n");
    check_fractions(10, 0.1, 0.2, 0.545678765, -1.0);
}
