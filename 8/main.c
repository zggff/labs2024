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
    poly_print(sqr);
    printf("\n");
    poly_free(p);
    poly_free(sqr);
    return 0;
}
