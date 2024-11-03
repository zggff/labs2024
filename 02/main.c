#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int is_sep(int c) {
    return isspace(c) || c == '(' || c == ')' || c == ',' || c == ';';
}

int parse_index(const char **s) {
    while (**s && isspace(**s))
        (*s)++;
    char c = tolower(**s);
    if ('a' <= c && c <= 'z') {
        (*s) += 2;
        return c - 'a';
    }
    return -1;
}

char *parse_str(const char **s) {
    while (**s && isspace(**s))
        (*s)++;
    const char *end = *s;
    while (*end && !is_sep(*end))
        end++;
    int n = end - *s;
    char *res = malloc(n + 1);
    if (!res)
        return NULL;
    memcpy(res, *s, n);
    for (int i = 0; i < n; i++) {
        res[i] = tolower(res[i]);
    }
    res[n] = 0;
    *s = end + 1;
    return res;
}

long parse_uint(const char **s) {
    while (**s && isspace(**s))
        (*s)++;
    char *ptr;
    size_t r = strtoul(*s, &ptr, 10);
    if (!is_sep(*ptr))
        return -1;
    *s = ptr + 1;
    return r;
}

typedef struct Arr {
    size_t size;
    unsigned *vals;
} Arr;

typedef int (*handle)(Arr *vars, const char **s);
#define ARRAY_CNT 26

typedef enum Status { S_OK, S_MALLOC, S_PARSE } Status;

int handle_load(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_save(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_rand(Arr *vars, const char **s) {
    int index, count, lb, rb;
    if ((index = parse_index(s)) < 0 || *(*s - 1) != ',' || // first
        (count = parse_uint(s)) < 0 || *(*s - 1) != ',' ||  // second
        (lb = parse_uint(s)) < 0 || *(*s - 1) != ',' ||     // third
        (rb = parse_uint(s)) < 0 || *(*s - 1) != ';' ||     // last
        rb < lb) {
        return S_PARSE;
    }
    Arr *a = &vars[index];
    a->vals = malloc(sizeof(int) * count);
    if (!a->vals)
        return S_MALLOC;
    a->size = count;
    for (size_t i = 0; i < a->size; i++) {
        a->vals[i] = rand() % (rb - lb + 1) + lb;
    }
    return S_OK;
}
int handle_concat(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_free(Arr *vars, const char **s) {
    int index = parse_index(s);
    if (index < 0 || *(*s - 1) != ')' || **s != ';')
        return S_PARSE;
    Arr *a = &vars[index];
    if (a->size > 0) {
        a->size = 0;
        free(a->vals);
    }
    return S_OK;
}
int handle_remove(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_copy(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_sort(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_shuffle(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_stats(Arr *vars, const char **s) {
    (void)vars;
    (void)s;
    return 0;
}
int handle_print(Arr *vars, const char **s) {
    int index = parse_index(s);
    if (index < 0 || *(*s - 1) != ',')
        return S_PARSE;

    const char *s_copy = *s;
    char *arg2_str = parse_str(&s_copy);
    int arg_2 = parse_uint(s);

    if ((arg_2 < 0 || (*(*s - 1) != ',' && *(*s - 1) != ';')) &&
        (strcmp(arg2_str, "all") != 0 || *(s_copy - 1) != ';')) {
        return S_PARSE;
    }
    free(arg2_str);

    Arr *a = &vars[index];
    int l, r;
    if (arg_2 < 0) { // print a, all;
        l = 0;
        r = a->size - 1;
    } else if (*(*s - 1) == ';') { // print a, n;
        l = 0;
        r = arg_2;
    } else {
        l = arg_2;
        r = parse_uint(s);
        if (r < 0 || *(*s - 1) != ';')
            return S_PARSE;
    }
    if (r >= (int)a->size) {
        fflush(stdout);
        fprintf(stderr,
                "ERROR: array overflow: attempted to access index [%d] of an "
                "array with lenth [%zu]\n",
                r, a->size);
        fflush(stderr);
        return S_OK;
    }
    printf("%c = [", index + 'A');
    for (int i = l; i <= r; i++) {
        if (i > l)
            printf(", ");
        printf("%d", a->vals[i]);
    }
    printf("]\n");
    return S_OK;
}

int main(void) {
    srand((unsigned int)time(NULL));
    size_t line_len = 0;
    char *line = NULL;
    const char *ops[] = {"load", "save", "rand",    "concat", "free", "remove",
                         "copy", "sort", "shuffle", "stats",  "print"};
    handle handles[] = {handle_load,   handle_save, handle_rand,
                        handle_concat, handle_free, handle_remove,
                        handle_copy,   handle_sort, handle_shuffle,
                        handle_stats,  handle_print};

    Arr arrays[ARRAY_CNT] = {0};
    while (true) {
        int n = getline(&line, &line_len, stdin);
        if (n <= 1)
            break;
        n--;
        line[n] = 0;
        const char *s = line;
        char *op = parse_str(&s);
        if (*op == 0)
            continue;
        handle *hand = NULL;
        for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
            if (strcmp(op, ops[i]) == 0) {
                hand = &handles[i];
                break;
            }
        }
        bool is_free = strcmp("free", op) == 0;
        free(op);
        if (is_free && *(s - 1) != '(') {
            fflush(stdout);
            fprintf(stderr, "ERROR: braces are expected as separator with "
                            "free command\n");
            fflush(stderr);
            continue;
        }

        if (!is_free && !isspace(*(s - 1))) {
            fflush(stdout);
            fprintf(stderr, "ERROR: commands must be followed by whitespace\n");
            fflush(stderr);
            continue;
        }

        if (hand == NULL) {
            fflush(stdout);
            fprintf(stderr, "ERROR: unknown operation [%s]\n", line);
            fprintf(stderr, "supported operations: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
                fprintf(stderr, "%s, ", ops[i]);
            }
            fprintf(stderr, "}\n");
            fflush(stderr);
            continue;
        }

        int r = (*hand)(arrays, &s);
        if (r == S_MALLOC) {
            fflush(stdout);
            fprintf(stderr, "ERROR: buy more ram\n");
            return 1;
        }
        if (r == S_PARSE) {
            fflush(stdout);
            fprintf(stderr, "ERROR: failed to parse arguments from line [%s]\n",
                    line);
            fflush(stderr);
        }
    }
    free(line);
    return 0;
}
