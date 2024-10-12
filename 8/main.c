#include "lib.h"
#include <stdio.h>

int main(void) {
    char *s = "2x^2 + 4x - 12";
    Poly p = {0};
    int r = poly_parse(&p, s);
    if (r) {
        printf("%d\n", r);
        return 1;
    }
    poly_print(p);
    printf("\n");
    Poly sqr;
    r = poly_pow(&sqr, p, 7);
    if (r) {
        printf("%d\n", r);
        return 2;
    }
    printf("\n");
    poly_print(sqr);
    printf("\n");
    Poly diff;
    poly_diff(&diff, sqr);

    printf("\n");
    poly_print(diff);
    printf("\n");

    double a, b, c;
    double x = 1.124;
    poly_eval(&a, p, x);
    poly_eval(&b, sqr, x);
    poly_eval(&c, diff, x);
    printf("\n%f %f %f\n", a, b, c);

    poly_free(p);
    poly_free(sqr);
    poly_free(diff);
    return 0;
}
