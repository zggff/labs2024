#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <time.h>
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
    S_OK = 0,
    S_MALLOC_ERROR = 1,
    S_PARSE_ERROR = 2,
} Status;

typedef struct tm tm;

typedef struct Liver {
    char *last_name;
    char *first_name;
    char *patronymic;
    tm date_of_birth;
    char gender;
    float income;
} Liver;

int liver_from_str(Liver *l, const char *s);
int liver_free(Liver *l);
int liver_print(const Liver *l);
int liver_write(const Liver *l, FILE *f);

typedef int (*mask)(int);

typedef struct Db {
    size_t size;
    size_t cap;
    Liver *ptr;
} Db;

int db_init(Db *d);
int db_push(Db *d, Liver);
int db_read_file(Db *d, FILE *f);
int db_free(Db *d);
int db_write(const Db *d, FILE *f);
int db_print(const Db *d);

int parse_field_str(char **res, char const **start, mask m);
int parse_field_uint(unsigned long *res, char const **start, mask m);
int parse_field_float(float *res, char const **start, mask m);
int parse_field_time(tm *res, const char **start, mask m);
int parse_field_char(char *res, const char **start, mask m);


#endif
