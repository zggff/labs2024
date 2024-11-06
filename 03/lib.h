#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

typedef struct Monom {
    double coef;
    Trie vars;
} Monom;

int monom_print(const Monom *m);
int monom_parse_str(Monom *p, const char *s);
int monom_parse_tokens(Monom *p, char **toks, int toks_len, int *off);

typedef struct Polynom Polynom;
struct Polynom {
    Polynom *prev;
    Monom cur;
    Polynom *next;
};

int polynom_free(Polynom *p);
int polynom_parse_str(Polynom *p, const char *s);
int polynom_parse_tokens(Polynom *p, char **toks, int toks_len, int *off);
int polynom_print(const Polynom *p);
