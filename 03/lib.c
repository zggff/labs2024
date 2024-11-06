#include "lib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize.h"
#include "trie.h"

int monom_print_callback(const char *k, int val, void *ptr) {
    (void)ptr;
    printf(" * %s^%d", k, val);
    return S_OK;
}

int monom_print(const Monom *m) {
    printf("%f", m->coef);
    return trie_for_each(&m->vars, monom_print_callback, NULL);
}

int monom_parse_tokens(Monom *p, char **toks, int toks_len) {
    p->coef = 1;
    bool neg = false;
    for (int i = 0; i < toks_len; i++) {
        if (strcmp(toks[i], "-") == 0 && i + 1 < toks_len &&
            isnumber(toks[i + 1][0])) {
            i++;
            neg = true;
        }
        if (isnumber(toks[i][0])) {
            char *end;
            double c = strtof(toks[i], &end);
            if (*end != 0) {
                fprintf(stderr, "ERROR: failed to parse [%s] as float\n",
                        toks[i]);
                fflush(stderr);
                return S_INVALID_INPUT;
            }
            if (neg)
                c *= -1;
            p->coef *= c;

        } else if (isalpha(toks[i][0])) {
            int power = 1;
            char *name = toks[i];
            if (i + 2 < toks_len && strcmp(toks[i + 1], "^") == 0) {
                if (i + 3 < toks_len && strcmp(toks[i + 2], "-") == 0) {
                    neg = true;
                    i++;
                }
                char *s_power = toks[i + 2];
                char *end;
                power = strtol(s_power, &end, 10);
                if (*end != 0) {
                    fprintf(stderr, "ERROR: failed to parse [%s] as int\n",
                            s_power);
                    fflush(stderr);
                    return S_INVALID_INPUT;
                }
                if (neg)
                    power *= -1;
                i += 2;
            }
            int current = trie_get(&p->vars, name);
            trie_set(&p->vars, name, current + power);
        } else {
            fprintf(stderr, "ERROR: unexpected token [%s]\n", toks[i]);
            fflush(stderr);
            return S_INVALID_INPUT;
        }
        if (i + 1 < toks_len) {
            if (strcmp(toks[i + 1], "*") != 0) {
                fprintf(stderr,
                        "ERROR: expected [*] after monom coefficient\n");
                fflush(stderr);
                return S_INVALID_INPUT;
            }
            i++;
        }
        neg = false;
    }
    return S_OK;
}

int monom_parse_str(Monom *p, const char *s) {
    size_t tok_len = 0;
    char **toks = NULL;
    int off = 0;
    int n = token_parse_list(&toks, &tok_len, s, &off);
    token_print(stdout, toks);
    int res = monom_parse_tokens(p, toks, n);
    for (int i = 0; i < n; i++) {
        free(toks[i]);
    }
    free(toks);
    return res;
}
