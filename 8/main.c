#include "lib.h"
#include <stdio.h>

int main(void) {
    char *s = "x^3 + x^2 - x^5 + 12x^124 - 12x + 415.1";
    Poly p = {0};
    int r = poly_parse(&p, s);
    printf("{%d}\n", r);
    poly_print(&p);
    return 0;
}
