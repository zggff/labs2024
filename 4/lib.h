#ifndef __LIB_H__
#define __LIB_H__

#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#define check(MACRO)                                                           \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            return r;                                                          \
        }                                                                      \
    }

typedef enum Status {
    S_OK = 0,
    S_NOT_EMPTY = 1,
    S_MALLOC = 2,
    S_OUT_OF_BOUNDS = 3,
    S_PARSE_ERROR = 4,
} Status;

typedef struct String {
    int len;
    char *ptr;
} String;

int string_from_str(String *s, const char *str);
int string_free(String *s);
int string_compare(int *res, const String *a, const String *b);
int string_compare_str(int *res, const String *a, const char *b);
int string_equal(bool *res, const String *a, const String *b);
int string_equal_str(bool *res, const String *a, const char *b);
int string_copy(String *a, const String *b);
int string_create_copy(String **a, const String *b);
int string_concatenate(String *a, const String *b);

int parse_field_str(String *res, char **start, const char *sep);
int parse_field_uint(unsigned *res, char **start, const char *sep);
int parse_field_float(float *res, char **start, const char *sep);

int parse_time(time_t *t, const String *a);

typedef struct Adress {
    String city;
    String street;
    unsigned building;
    String block;
    unsigned flat;
    String id;
} Adress;

int address_from_string(Adress *a, char **s);

typedef struct Mail {
    Adress addr;
    float weight;
    String id;
    String create;
    String receive;
} Mail;

// int cmp_time(int *res, const struct tm * a, const struct tm* t);

int mail_print(const Mail *m);
int mail_cmp_id(const void *a0, const void *b0);
int mail_cmp_date(const void *a0, const void *b0);
int mail_from_string(Mail *m, char **s);

typedef struct Post {
    Adress *current;
    size_t size;
    size_t capacity;
    Mail *mail;
} Post;

int post_init(Post *p);
int post_print(const Post *p);
int post_add(Post *p, Mail m);
int post_remove(Post *p, size_t i);
int post_sort(Post *p);
int post_filter_by_delivery_status(Post *p, const Post *p0, bool delivered);
int post_free(Post *p);

#endif
