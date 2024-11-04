#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

int monom_print_callback(const char *k, int val, void * ptr) {
    (void) ptr;
    printf(" * %s^%d", k, val);
    return S_OK;
}

int monom_print(const Monom *m) {
    printf("%f", m->coef);
    return trie_for_each(&m->vars, monom_print_callback, NULL);
}
