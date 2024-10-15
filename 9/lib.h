#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>

typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_PARSE_ERROR,
    S_INVALID_INPUT,
    S_FILE_READ_ERROR
} Status;

typedef int (*mask)(int);
size_t getword(char **res, size_t *cap, FILE *f, const char *sep);
int parse_field_str(char **res, char const **start, mask m);
int parse_field_uint(unsigned long *res, char const **start, mask m);

typedef struct Leaf {
    char *s;
    size_t cnt;
} Leaf;

typedef struct Tree Tree;
struct Tree {
    Leaf v;
    Tree *l;
    Tree *r;
};

int tree_parse_file(Tree *t, const char *sep, FILE *f);
int tree_add(Tree *t, const char *word);
int tree_get(const Tree *t, const char *word);
int tree_free(Tree *t);
int tree_print(const Tree *t);
int tree_size(const Tree *t);
int tree_dump_to_sorted_list(Leaf **r, const Tree *t);
int tree_longest(char **r, const Tree *t);
int tree_shortest(char **r, const Tree *t);
int tree_depth(const Tree *t);
int tree_write(FILE *f, const Tree *t);
int tree_read(FILE *f, Tree *t);

#endif
