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

int parse_field_str(char **res, char const **start, mask m) {
    const char *end = *start;
    while (*end && !m(*end)) {
        end++;
    }
    int n = end - *start;
    *res = malloc(n + 1);
    if (!res)
        return S_MALLOC_ERROR;
    memcpy(*res, *start, n);
    (*res)[n] = 0;
    *start = end + 1;
    return S_OK;
}

int parse_field_uint(unsigned long *res, char const **start, mask m) {
    char *ptr;
    *res = strtoul(*start, &ptr, 10);
    bool valid = *ptr == 0;
    valid = valid | m(*ptr);
    if (!valid) {
        ptr++;
        char tmp = *ptr;
        *ptr = 0;
        fprintf(stderr, "ERROR: failed to parse [%s] as unsigned\n", *start);
        *ptr = tmp;
        return S_PARSE_ERROR;
    }

    *start = ptr + 1;
    return S_OK;
}

int _tree_inc_(Tree *t, const char *word) {
    if (!t->v.s) {
        t->v.s = malloc(strlen(word) + 1);
        if (!t->v.s)
            return S_MALLOC_ERROR;
        strcpy(t->v.s, word);
    }
    t->v.cnt++;
    return S_OK;
}

int tree_add(Tree *t, const char *word) {
    if (t->v.s == NULL)
        return _tree_inc_(t, word);

    int cmp;
    Tree **c = &t;
    do {
        cmp = strcmp(word, (*c)->v.s);
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
    if (t->v.s == NULL)
        return -1;
    int cmp;
    do {
        cmp = strcmp(word, t->v.s);
        if (cmp < 0)
            t = t->l;
        if (cmp > 0)
            t = t->r;
    } while (cmp && t);
    if (!t)
        return -1;
    return t->v.cnt;
}

int _tree_print(const Tree *t, int level) { // can't implement non recursively
    if (!t)
        return 0;
    _tree_print(t->l, level + 1);
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("[%s]: %zu\n", t->v.s, t->v.cnt);
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
    free(t->v.s);
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

int tree_size(const Tree *t) {
    if (!t)
        return 0;
    return tree_size(t->l) + tree_size(t->r);
}

int _push_to_list(Leaf *r, const Tree *t) {
    if (!t)
        return 0;
    _push_to_list(r, t->l);
    _push_to_list(r, t->r);
    return 0;
}

int leaf_cmp(const void *p_a, const void *p_b) {
    const Leaf *a = p_a;
    const Leaf *b = p_b;
    return (a->cnt > b->cnt) - (a->cnt < b->cnt);
}

int tree_dump_to_sorted_list(Leaf **r, const Tree *t) {
    int n = tree_size(t);
    *r = malloc(n * sizeof(Leaf));
    if (!*r)
        return -1;
    _push_to_list(*r, t);
    qsort(*r, n, sizeof(Leaf), leaf_cmp);
    return 0;
}
