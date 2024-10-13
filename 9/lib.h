#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>

typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_PARSE_ERROR,
} Status;

size_t getword(char **res, size_t *cap, FILE *f, const char *sep);

typedef struct Tree Tree;
struct Tree {
    char *s;
    size_t cnt;
    Tree *l;
    Tree *r;
};

int tree_parse_file(Tree *t, const char *sep, FILE *f);
int tree_add(Tree *t, const char *word);
int tree_get(const Tree *t, const char *word);
int tree_free(Tree *t);
int tree_print(const Tree *t);

#endif
