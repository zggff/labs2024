#include "lib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t getword(char **res, size_t *cap, FILE *f, const char *sep) {
    if (*res == NULL) {
        *cap = 32;
        *res = malloc(*cap);
        if (!*res)
            return -1;
    }
    size_t i = 0;
    bool found = false;
    while (!found) {
        char c = fgetc(f);
        if (c == 0 || c == EOF)
            break;

        for (const char *s = sep; *s && !found; s++)
            found = found || c == *s;

        if (i >= *cap - 1) {
            size_t new_cap = *cap * 2;
            char *t = realloc(*res, new_cap);
            if (!t)
                return -1;
            *cap = new_cap;
            *res = t;
        }
        (*res)[i] = c;
        i++;
    }
    (*res)[i] = 0;
    return i;
}
