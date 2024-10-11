#include "lib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int handle_print(Db *d, const char *s) {
    (void)s;
    return db_print(d);
}
int handle_push(Db *d, const char *s) {
    Liver l = {0};
    check(liver_from_str(&l, s), {});
    return db_push(d, l);
}
int handle_remove(Db *d, const char *s) {
    size_t i;
    check(parse_field_uint(&i, &s, isspace), {});
    return db_remove(d, i);
}

int handle_update_first_name(Liver *l, const char *s) {
    free(l->first_name);
    check(parse_field_str(&l->first_name, &s, isspace), liver_free(l));
    bool valid;
    validate_name(&valid, l->first_name);
    if (!valid || l->first_name[0] == 0) {
        fprintf(stderr,
                "ERROR: first name must not be empty and can only "
                "contain latin symbols: [%s]\n",
                l->first_name);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

int handle_update_last_name(Liver *l, const char *s) {
    free(l->last_name);
    check(parse_field_str(&l->last_name, &s, isspace), liver_free(l));
    bool valid;
    validate_name(&valid, l->last_name);
    if (!valid || l->last_name[0] == 0) {
        fprintf(stderr,
                "ERROR: last name must not be empty and can only "
                "contain latin symbols: [%s]\n",
                l->last_name);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

int handle_update_patronymic(Liver *l, const char *s) {
    free(l->patronymic);
    check(parse_field_str(&l->patronymic, &s, isspace), liver_free(l));
    bool valid;
    validate_name(&valid, l->patronymic);
    if (!valid) {
        fprintf(stderr,
                "ERROR: patronymic can only "
                "contain latin symbols: [%s]\n",
                l->patronymic);
        liver_free(l);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

int handle_update_date_of_birth(Liver *l, const char *s) {
    check(parse_field_time(&l->date_of_birth, &s, isspace), {});
    return S_OK;
}

int handle_update_gender(Liver *l, const char *s) {
    check(parse_field_char(&l->gender, &s, isspace), {});
    if (l->gender != 'M' && l->gender != 'W') {
        fprintf(stderr, "ERROR: gender can be either 'M' or 'W': [%c]\n",
                l->gender);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

int handle_update_income(Liver *l, const char *s) {
    check(parse_field_float(&l->income, &s, isspace), {});
    if (l->income < 0) {
        fprintf(stderr, "ERROR: income must be positive: [%f]\n", l->income);
        return S_PARSE_ERROR;
    }
    return S_OK;
}

typedef int (*handle_liver_update)(Liver *l, const char *s);

int handle_update(Db *d, const char *s) {
    size_t i;
    char *op;
    check(parse_field_uint(&i, &s, isspace), {});
    check(parse_field_str(&op, &s, isspace), {});
    if (i >= d->size) {
        fprintf(stderr,
                "ERROR: out of bounds: tried to access index [%zu] in an array "
                "of length [%zu]\n",
                i, d->size);
        return S_OUT_OF_BOUNDS_ERROR;
    }
    Liver l = {0};
    liver_copy(&l, &d->ptr[i]);
    const char *ops[] = {"first_name",    "last_name", "patronymic",
                         "date_of_birth", "gender",    "income"};
    handle_liver_update handles[] = {
        handle_update_first_name, handle_update_last_name,
        handle_update_patronymic, handle_update_date_of_birth,
        handle_update_gender,     handle_update_income};

    bool found = false;
    for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
        if (strcmp(op, ops[i]) == 0) {
            check(handles[i](&l, s), free(op));
            found = true;
            break;
        }
    }
    if (!found) {
        fprintf(stderr, "ERROR: unknown field [%s]\n", op);
        fprintf(stderr, "supported fields: {");
        for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
            fprintf(stderr, "%s, ", ops[i]);
        }
        fprintf(stderr, "}\n");
        free(op);
        return S_INPUT_ERROR;
    }

    free(op);
    return db_update(d, i, l);
}

int handle_write(Db *d, const char *s) {
    char *fname;
    check(parse_field_str(&fname, &s, isspace), {});
    if (fname[0] == 0) {
        fprintf(stderr, "ERROR: filename not provided\n");
        free(fname);
        return S_INPUT_ERROR;
    }
    FILE *f = fopen(fname, "w");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file [%s]\n", fname);
        free(fname);
        return S_FILE_ERROR;
    }
    db_write(d, f);
    fclose(f);
    free(fname);
    return S_OK;
}
int handle_undo(Db *d, const char *s) {
    (void)s;
    return db_undo(d);
}
// int handle_find(Db *d, const char *s);

typedef int (*handle)(Db *f, const char *s);

int main(int argc, const char *argw[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: no input file provided\n");
        return 1;
    }
    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[1]);
        return 1;
    }

    Db d = {0};
    db_init(&d);
    db_read_file(&d, f);

    const char *ops[] = {"print", "write", "push", "remove", "update", "undo"};
    handle handles[] = {handle_print,  handle_write,  handle_push,
                        handle_remove, handle_update, handle_undo};

    char *line = NULL;
    size_t line_len = 0;
    while (true) {
        int n = getline(&line, &line_len, stdin);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;

        const char *s = line;
        char *op;
        check(parse_field_str(&op, &s, isspace), {});
        bool found = false;
        for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
            if (strcmp(op, ops[i]) == 0) {
                handles[i](&d, s);
                found = true;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "ERROR: unknown operation [%s]\n", line);
            fprintf(stderr, "supported operations: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
                fprintf(stderr, "%s, ", ops[i]);
            }
            fprintf(stderr, "}\n");
        }
        free(op);
    }
    free(line);

    db_free(&d);

    fclose(f);
    return 0;
}
