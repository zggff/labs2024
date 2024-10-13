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

#endif
