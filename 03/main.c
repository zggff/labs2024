#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "tokenize.h"

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

    char **toks = NULL;
    size_t tok_cap = 0;
    char buf[BUF_SIZE] = {0};
    int off = 0;
    while (true) {
        int n = token_parse_file(&toks, &tok_cap, buf, &off, f);
        if (n <= 0)
            break;

        token_print(stdout, toks);
        for (int i = 0; i < n; i++)
            free(toks[i]);
    }
    free(toks);
    fclose(f);
    return 0;
}
