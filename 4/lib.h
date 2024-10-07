#ifndef __LIB_H__
#define __LIB_H__

#include <stdbool.h>

typedef enum Status {
    S_OK = 0,
    S_NOT_EMPTY = 1,
    S_MALLOC = 2,
} Status;

typedef struct String {
    int len;
    char *ptr;
} String;

int string_from_str(String *s, const char *str);
int string_free(String *s);
int string_compare(int *res, const String *a, const String *b);
int string_equal(bool *res, const String *a, const String *b);
int string_copy(String *a, const String *b);
int string_create_copy(String **a, const String *b);
int string_concatenate(String *a, const String *b);

#endif
