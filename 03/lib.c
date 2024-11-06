#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize.h"
#include "trie.h"
#include "lib.h"

int monom_print_callback(const char *k, int val, void *ptr) {
    (void)ptr;
    printf(" * %s^%d", k, val);
    return S_OK;
}

int monom_print(const Monom *m) {
    printf("%f", m->coef);
    return trie_for_each(&m->vars, monom_print_callback, NULL);
}

int monom_parse_tokens(Monom *p, char **toks, int toks_len, int *off) {
    p->coef = 1;
    bool neg = false;
    if (*off < toks_len && strcmp(toks[*off], "-") == 0) {
        p->coef *= -1;
        (*off)++;
    }
    int i;
    for (i = *off; i < toks_len; i++) {
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
        i++;
        if (i < toks_len) {
            if (strcmp(toks[i], "*") != 0) {
                break;
            }
        }
        neg = false;
    }
    *off = i;
    return S_OK;
}

int polynom_add_monom(Polynom *p, Monom m) {
    // monom_print(&m);
    // printf("\n");
    Polynom *tmp = p;
    Polynom *prev = NULL;
    while (tmp) {
        if (tmp->cur.coef == 0 || trie_eq(&tmp->cur.vars, &m.vars))
            break;
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp) {
        if (tmp->cur.coef == 0)
            tmp->cur = m;
        else
            tmp->cur.coef += m.coef;

        return S_OK;
    }
    prev->next = malloc(sizeof(Polynom));
    if (prev->next == NULL)
        return S_MALLOC;
    prev->next->cur = m;
    prev->next->next = NULL;
    prev->next->prev = prev;
    return S_OK;
}

int polynom_parse_tokens(Polynom *p, char **toks, int toks_len, int *off) {
    bool end = false;
    while (!end) {
        Monom m = {0};
        int r = monom_parse_tokens(&m, toks, toks_len, off);
        if (r)
            return r;
        bool valid = false;
        if (*off >= toks_len || strcmp(toks[*off], ",") == 0 ||
            strcmp(toks[*off], ")") == 0) {
            valid = true;
            end = true;
        }
        if (!valid &&
            (strcmp(toks[*off], "+") == 0 || strcmp(toks[*off], "-") == 0)) {
            valid = true;
        }
        if (*off < toks_len && strcmp(toks[*off], "+") == 0) {
            (*off)++;
        }
        if (!valid) {
            fprintf(stderr, "ERROR: unexpected token [%s]\n", toks[*off]);
            fflush(stderr);
            return S_INVALID_INPUT;
        }
        r = polynom_add_monom(p, m);
        if (r)
            return r;
    }
    return S_OK;
}

int polynom_parse_str(Polynom *p, const char *s) {
    size_t tok_len = 0;
    char **toks = NULL;
    int off = 0;
    int n = token_parse_list(&toks, &tok_len, s, &off);
    int res_off = 0;
    int res = polynom_parse_tokens(p, toks, n, &res_off);
    for (int i = 0; i < n; i++) {
        free(toks[i]);
    }
    free(toks);
    return res;
}

int monom_parse_str(Monom *p, const char *s) {
    size_t tok_len = 0;
    char **toks = NULL;
    int off = 0;
    int n = token_parse_list(&toks, &tok_len, s, &off);
    int res_off = 0;
    int res = monom_parse_tokens(p, toks, n, &res_off);
    for (int i = 0; i < n; i++) {
        free(toks[i]);
    }
    free(toks);
    return res;
}

int polynom_print(const Polynom *p) {
    int i = 0;
    while (p) {
        if (i > 0)
            printf(" + ");
        monom_print(&p->cur);
        i = 1;
        p = p->next;
    };
    printf("\n");
    return S_OK;
}

int polynom_free(Polynom *p) {
    while (p) {
        Polynom *next = p->next;
        trie_free(&p->cur.vars);
        free(p);
        p = next;
    }
    return 0;
}
