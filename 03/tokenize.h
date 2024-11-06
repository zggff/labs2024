#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

char _getc(const char *buf, int *off) {
    if (buf[*off] == 0)
        return 0;
    char res = buf[*off];
    (*off)++;
    return res;
}

int _is_sep(char c) {
    return isspace(c) || c == '-' || c == '+' || c == '*' || c == '^' ||
           c == ',' || c == ';' || c == '(' || c == ')';
}

long token_get(char **s, size_t *cap, const char *buf, int *off) {
    if (*s == NULL) {
        *cap = 32;
        *s = malloc(*cap);
        if (*s == NULL)
            return 0;
    }
    char c;
    do {
        c = _getc(buf, off);
    } while (c && isspace(c));

    if (!c) {
        (*s)[0] = 0;
        return 0;
    }

    size_t i = 0;
    do {
        if (i >= *cap - 1) {
            size_t new_cap = *cap * 2;
            char *s2 = realloc(*s, new_cap);
            if (s2 == NULL) {
                free(s);
                return 0;
            }
            *cap = new_cap;
            *s = s2;
        }
        (*s)[i] = tolower(c);
        i++;
        c = _getc(buf, off);
    } while (c && !_is_sep(c) && !_is_sep((*s)[i - 1]));
    if (c != 0)
        (*off)--;

    (*s)[i] = 0;
    return i;
}

long token_parse_list(char ***s, size_t *s_cap, const char *buf, int *off) {
    if (*s == NULL) {
        *s_cap = 32;
        *s = malloc(*s_cap * sizeof(char *));
        if (*s == NULL)
            return 0;
        memset(*s, 0, *s_cap * sizeof(char *));
    }

    size_t i = 0;
    size_t tok_len = 0;
    char *tok = NULL;
    while (true) {
        if (i >= *s_cap - 1) {
            size_t new_cap = *s_cap * 2;
            char **s2 = realloc(*s, new_cap * sizeof(char *));
            if (s2 == NULL) {
                free(s);
                return 0;
            }
            *s = s2;
            memset(*s + *s_cap, 0, (new_cap - *s_cap) * sizeof(char *));
            *s_cap = new_cap;
        }

        int n = token_get(&tok, &tok_len, buf, off);
        if (n <= 0)
            break;
        (*s)[i] = malloc(n + 1);
        memcpy((*s)[i], tok, n + 1);
        i++;
    }
    free(tok);
    return i;
}

int token_print_error(FILE *f, const char *tok, const char *exp) {
    if (*tok)
        fprintf(f, "ERROR: expected [%s] got [%s]\n", exp, tok);
    else
        fprintf(f, "ERROR: expected [%s] got none\n", exp);
    fflush(f);
    return 0;
}

int token_print(FILE *f, char **tok) {
    fprintf(f, "{");
    for (size_t i = 0; tok[i]; i++) {
        if (i > 0)
            fprintf(f, " ");
        fprintf(f, "[%s]", tok[i]);
    }
    fprintf(f, "}\n");
    return 0;
}
