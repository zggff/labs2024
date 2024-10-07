#ifndef __LIB_H__
#define __LIB_H__

#include <stddef.h>
#include <stdbool.h>

typedef enum Status {
    S_OK = 0,
    S_NOT_EMPTY = 1,
    S_MALLOC = 2,
    S_OUT_OF_BOUNDS = 3,
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

typedef struct Adress {
    String city;
    String street;
    unsigned building;
    String block;
    unsigned flat;
    String index;
} Adress;

typedef struct Mail {
    Adress receiver;
    float weight;
    String id;
    String create_time;
    String receive_time;
} Mail;

int mail_print(const Mail *m);
int mail_cmp(const void *a, const void *b);

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

#endif
