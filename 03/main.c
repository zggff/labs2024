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

typedef int (*handle)(Polynom *p, char **toks, int tok_len);

int handle_two(Polynom *p, char **toks, int tok_len) {
    const char *command = toks[0];
    Polynom a = {0};
    Polynom b = {0};
    if (tok_len < 1 || strcmp(toks[1], "(") != 0) {
        token_print_error(stderr, toks[1], "(");
        return S_INVALID_INPUT;
    }
    tok_len -= 2;
    toks += 2;
    int off = 0;
    int r = polynom_parse_tokens(&a, toks, tok_len, &off);
    if (r)
        return r;
    toks += off;
    tok_len -= off;
    off = 0;
    if (strcmp(toks[0], ",") == 0) {
        polynom_free(p);
        int r = polynom_parse_tokens(&b, toks + 1, tok_len - 1, &off);
        if (r) {
            polynom_free(&a);
            return r;
        }
        toks += off + 1;
        tok_len -= off + 1;
    } else if (strcmp(toks[0], ")") == 0) {
        b = a;
        a = *p;
        toks++;
        tok_len--;
    }

    memset(p, 0, sizeof(Polynom));
    int res = 0;
    if (strcmp(command, "Add") == 0) {
        res = polynom_add(p, &a, &b);
    } else if (strcmp(command, "Sub") == 0) {
        res = polynom_sub(p, &a, &b);
    } else {
        res = polynom_mul(p, &a, &b);
    }
    polynom_print(p);
    polynom_free(&a);
    polynom_free(&b);
    return res;
}

int handle_ni(Polynom *p, char **toks, int tok_len) {
    (void)p;
    (void)toks;
    (void)tok_len;
    fprintf(stderr, "ERROR: [%s] not implemented\n", toks[0]);
    return -1;
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
    handle handles[] = {handle_two, handle_two, handle_two, handle_ni,
                        handle_ni,  handle_ni,  handle_ni};

    char **toks = NULL;
    size_t tok_cap = 0;
    char buf[BUF_SIZE] = {0};
    int off = 0;

    Polynom p = {0};

    int res = 0;

    while (!res) {
        int n = token_parse_file(&toks, &tok_cap, buf, &off, f);
        if (n <= 0)
            break;
        handle *h = NULL;
        for (size_t i = 0; !h && i < sizeof(ops) / sizeof(char *); i++) {
            if (strcmp(toks[0], ops[i]) == 0)
                h = &handles[i];
        }
        if (h) {
            res = (*h)(&p, toks, n);
            fflush(stdout);
        } else {
            fprintf(stderr, "ERROR: unsupported operation: {%s}\n", toks[0]);
            fprintf(stderr, "\tsupported: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(char *); i++) {
                if (i > 0)
                    fprintf(stderr, ", ");
                fprintf(stderr, "%s", ops[i]);
            }
            fprintf(stderr, "}\n");
            fflush(stderr);
            res = S_INVALID_INPUT;
        }

        for (int i = 0; i < n; i++) {
            free(toks[i]);
            toks[i] = NULL;
        }
    }
    polynom_free(&p);
    free(toks);
    fclose(f);
    return res;
}
