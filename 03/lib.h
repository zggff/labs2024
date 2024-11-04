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

typedef struct Polynom Polynom;
struct Polynom {
    Polynom *prev;
    Monom cur;
    Polynom *next;
};
