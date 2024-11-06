#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "trie.h"


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

    Monom m = {0};
    int r = monom_parse_str(&m, "12 * xy^12 * x21^4 * -2 * xy^-3");
    printf("r = %d\n", r);

    // m.coef = 12.4;
    // trie_set(&m.vars, "x", 12);
    // trie_set(&m.vars, "y", 3);
    monom_print(&m);
    printf("\n");
    // printf("%ld\n", trie_get(&m.vars, "y"));
    // printf("%ld\n", trie_get(&m.vars, "x"));


    fclose(f);
    return 0;
}
