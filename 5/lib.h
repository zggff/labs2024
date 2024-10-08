#ifndef __LIB_H__
#define __LIB_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>



typedef enum Status {
    S_OK = 0,
    S_MALLOC_ERROR = 1,
    S_PARSE_ERROR = 2,
} Status;

typedef struct Student {
    unsigned long id;
    char *first_name;
    char *last_name;
    char *group;
    unsigned char *grades;
} Student;

int student_from_str(Student *s, const char *str);
int student_free(Student *s);
int student_print(const Student *s);
int cmp_student_id(const void *a0, const void *b0);
int cmp_student_ln(const void *a0, const void *b0);
int cmp_student_fn(const void *a0, const void *b0);
int cmp_student_gr(const void *a0, const void *b0);

typedef struct Db {
    size_t cap;
    size_t size;
    float avg_grade;
    Student *ptr;
    bool not_original;
} Db;

int db_init(Db *d);
int db_free(Db *d);
int db_print(const Db *d);
int db_search_id(Db *res, const Db *d, unsigned long id);
int db_search_ln(Db *res, const Db *d, const char *ln);
int db_search_fn(Db *res, const Db *d, const char *fn);
int db_search_gr(Db *res, const Db *d, const char *gp);
int db_sort_id(const Db *d);
int db_sort_ln(const Db *d);
int db_sort_fn(const Db *d);
int db_sort_gr(const Db *d);
int db_from_file(Db *d, FILE *f);
int db_push(Db *d, Student s);

typedef int (*mask)(int);

int parse_field_str(char **res, char const **start, mask m);
int parse_field_uint(unsigned long *res, char const **start, mask m);

#endif
