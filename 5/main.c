#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

typedef int (*handle)(Db *d, const char *s, FILE *out);

int handle_print(Db *d, const char *s, FILE *out) {
    (void)s;
    (void)out;
    return db_print(d);
}

int handle_sort_id(Db *d, const char *s, FILE *out) {
    (void)s;
    (void)out;
    return db_sort_id(d);
}

int handle_sort_fn(Db *d, const char *s, FILE *out) {
    (void)s;
    (void)out;
    return db_sort_fn(d);
}
int handle_sort_ln(Db *d, const char *s, FILE *out) {
    (void)s;
    (void)out;
    return db_sort_ln(d);
}
int handle_sort_gr(Db *d, const char *s, FILE *out) {
    (void)s;
    (void)out;
    return db_sort_gr(d);
}
int handle_find_id(Db *d, const char *s, FILE *out) {
    (void)out;
    unsigned long id;
    int r;
    if ((r = parse_field_uint(&id, &s, isspace)))
        return r;
    Db filtered = {0};
    if ((r = db_init(&filtered)))
        return r;
    db_search_id(&filtered, d, id);
    db_print(&filtered);
    db_free(&filtered);
    return S_OK;
}
int handle_find_fn(Db *d, const char *s, FILE *out) {
    (void)out;
     char *fn;
    int r;
    Db filtered = {0};
    if ((r = db_init(&filtered)))
        return r;
    if ((r = parse_field_str(&fn, &s, isspace)))
        return r;
    db_search_fn(&filtered, d, fn);
    db_print(&filtered);
    db_free(&filtered);
    free(fn);
    return S_OK;
}
int handle_find_ln(Db *d, const char *s, FILE *out) {
    (void)out;
    char *ln;
    int r;
    Db filtered = {0};
    if ((r = db_init(&filtered)))
        return r;
    if ((r = parse_field_str(&ln, &s, isspace)))
        return r;
    db_search_ln(&filtered, d, ln);
    db_print(&filtered);
    db_free(&filtered);
    free(ln);
    return S_OK;
}
int handle_find_gr(Db *d, const char *s, FILE *out) {
    (void)out;
    char *gr;
    int r;
    Db filtered = {0};
    if ((r = db_init(&filtered)))
        return r;
    if ((r = parse_field_str(&gr, &s, isspace)))
        return r;
    db_search_gr(&filtered, d, gr);
    db_print(&filtered);
    db_free(&filtered);
    free(gr);
    return S_OK;
}
int handle_dump_into_file_by_id(Db *d, const char *s, FILE *out) {
    (void)out;
    unsigned long id;
    int r;
    if ((r = parse_field_uint(&id, &s, isspace)))
        return r;
    Db filtered = {0};
    if ((r = db_init(&filtered)))
        return r;
    db_search_id(&filtered, d, id);
    fprintf(out, "Students with id: %lu\n", id);
    for (size_t i = 0; i < filtered.size; i++) {
        Student s = filtered.ptr[i];
        float avg = 0;
        for (int j = 0; j < 5; j++)
            avg += s.grades[j];
        avg = avg / 5;
        fprintf(out, "%s %s %s %f\n", s.last_name, s.first_name, s.group, avg);
    }
    fprintf(out, "\n");
    db_free(&filtered);
    return S_OK;
}

int handle_dump_into_file_by_avg(Db *d, const char *s, FILE *out) {
    (void)s;
    fprintf(out,
            "Students with average grade higher than total average grade\n");
    for (size_t i = 0; i < d->size; i++) {
        Student s = d->ptr[i];
        float avg = 0;
        for (int j = 0; j < 5; j++)
            avg += s.grades[j];
        avg = avg / 5;
        if (avg > d->avg_grade)
            fprintf(out, "%s %s\n", s.last_name, s.first_name);
    }
    fprintf(out, "\n");
    return S_OK;
}

int main(int argc, const char *argw[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: not enough args provided\n");
        return 1;
    }
    FILE *in = fopen(argw[1], "r");
    if (!in) {
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", argw[1]);
        return 1;
    }
    FILE *out = fopen(argw[2], "w");
    if (!out) {
        fclose(in);
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", argw[2]);
        return 1;
    }

    const char *ops[] = {"Print",  "FindId", "FindFn", "FindLn",
                         "FindGr", "SortId", "SortFn", "SortLn",
                         "SortGr", "DumpId", "DumpAvg"};
    handle handles[] = {handle_print,
                        handle_find_id,
                        handle_find_fn,
                        handle_find_ln,
                        handle_find_gr,
                        handle_sort_id,
                        handle_sort_fn,
                        handle_sort_ln,
                        handle_sort_gr,
                        handle_dump_into_file_by_id,
                        handle_dump_into_file_by_avg};

    size_t line_len = 0;
    char *line = NULL;

    Db db = {0};
    if (db_init(&db)) {
        fprintf(stderr, "ERROR: failed to initialize array\n");
        return 1;
    }
    if (db_from_file(&db, in)) {
        fprintf(stderr, "ERROR: failed to parse students\n");
        return 1;
    }

    while (true) {
        int n = getline(&line, &line_len, stdin);
        if (n <= 1)
            break;
        n--;
        line[n] = 0;

        const char *s = line;
        char *op;
        parse_field_str(&op, &s, isspace);
        bool found = false;
        for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
            if (strcmp(op, ops[i]) == 0) {
                found = true;
                handles[i](&db, s, out);
                break;
            }
        }
        if (!found)
            fprintf(stderr, "ERROR: unknown operation [%s]\n", op);
        free(op);
    }
    db_free(&db);
    if (line)
        free(line);

    fclose(in);
    fclose(out);
    return 0;
}
