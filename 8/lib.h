#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int _r = MACRO;                                                        \
        if (_r) {                                                              \
            AFTER;                                                             \
            return _r;                                                         \
        }                                                                      \
    }

typedef enum Status {
    S_OK,
    S_MALLOC_ERROR,
    S_PARSE_ERROR,
} Status;

typedef int (*mask)(int);
int parse_field_str(char **res, char const **start, mask m);
int parse_field_float(float *res, char const **start, mask m);
int parse_field_uint(unsigned long *res, char const **start, mask m);
int parse_field_char(char *res, char const **start, mask m);

#endif
