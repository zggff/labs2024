#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "tokenize.h"

#define FREE_ALL()                                                             \
    {                                                                          \
        for (int i = 0; toks[i]; i++) {                                        \
            free(toks[i]);                                                     \
            toks[i] = NULL;                                                    \
        }                                                                      \
        free(toks);                                                            \
        fclose(f);                                                             \
    }

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

    char *ops[] = {"Add", "Sub", "Mult", "Eval", "Deriv", "Grad", "Prim"};

    char **toks = NULL;
    size_t tok_cap = 0;
    char buf[BUF_SIZE] = {0};
    int off = 0;
    while (true) {
        int n = token_parse_file(&toks, &tok_cap, buf, &off, f);
        if (n <= 0)
            break;
        bool found = false;
        for (size_t i = 0; !found && i < sizeof(ops) / sizeof(char *); i++) {
            if (strcmp(toks[0], ops[i]) == 0)
                found = true;
        }
        if (!found) {
            fprintf(stderr, "ERROR: unsupported operation: {%s}\n", toks[0]);
            fprintf(stderr, "\tsupported: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(char *); i++) {
                if (i > 0)
                    fprintf(stderr, ", ");
                fprintf(stderr, "%s", ops[i]);
            }
            fprintf(stderr, "}\n");
            fflush(stderr);
            FREE_ALL();
            return 2;
        }

        token_print(stdout, toks);
        fflush(stdout);
        for (int i = 0; i < n; i++)
            free(toks[i]);
    }
    free(toks);
    fclose(f);
    return 0;
}
