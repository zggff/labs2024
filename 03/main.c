#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

int main(int argc, char *argw[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: input file not provided");
        fflush(stderr);
        return 1;
    }
    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[1]);
        fflush(stderr);
        return 1;
    }

    Polynom p = {0};
    int r =
        polynom_parse_str(&p, "12 * xy^12 * x21^4 * -2 * xy^-3 + 12 *z -1 - x");
    printf("r = %d\n", r);
    polynom_print(&p);
    polynom_free(&p);

    fclose(f);
    return 0;
}
