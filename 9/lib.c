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

int _tree_inc_(Tree *t, const char *word) {
    if (!t->s) {
        t->s = malloc(strlen(word) + 1);
        if (!t->s)
            return S_MALLOC_ERROR;
        strcpy(t->s, word);
    }
    t->cnt++;
    return S_OK;
}

int tree_add(Tree *t, const char *word) {
    if (t->s == NULL)
        return _tree_inc_(t, word);

    int cmp;
    Tree **c = &t;
    do {
        cmp = strcmp(word, (*c)->s);
        if (cmp < 0)
            c = &(*c)->l;
        if (cmp > 0)
            c = &(*c)->r;
    } while (cmp && *c);

    if (!(*c)) {
        *c = malloc(sizeof(Tree));
        if (!*c)
            return S_MALLOC_ERROR;
        memset(*c, 0, sizeof(Tree));
    }

    return _tree_inc_(*c, word);
}

int tree_get(const Tree *t, const char *word) {
    if (t->s == NULL)
        return -1;
    int cmp;
    do {
        cmp = strcmp(word, t->s);
        if (cmp < 0)
            t = t->l;
        if (cmp > 0)
            t = t->r;
    } while (cmp && t);
    if (!t)
        return -1;
    return t->cnt;
}

int _tree_print(const Tree *t, int level) { // can't implement non recursively
    if (!t)
        return 0;
    _tree_print(t->l, level + 1);
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("[%s]: %zu\n", t->s, t->cnt);
    _tree_print(t->r, level + 1);
    return 0;
}

int tree_print(const Tree *t) {
    return _tree_print(t, 0);
}

int tree_free(Tree *t) { // can't implement non recursively
    if (!t)
        return 0;
    tree_free(t->l);
    tree_free(t->r);
    free(t->l);
    free(t->r);
    free(t->s);
    return 0;
}

int tree_parse_file(Tree *t, const char *sep, FILE *f) {
    char *word = NULL;
    size_t word_cap = 0;
    Status s = S_OK;
    while (s == S_OK) {
        int n = getword(&word, &word_cap, f, sep);
        if (n <= 0)
            break;
        if (n == 1)
            continue;
        word[n - 1] = 0;
        s = tree_add(t, word);
    }
    free(word);
    return s;
}
