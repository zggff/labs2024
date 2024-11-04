#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VAR_CNT 26
#define BUF_SIZE 128

long _refill_buffer(char buffer[BUF_SIZE], int *buf_off, FILE *f) {
    if (*buf_off >= BUF_SIZE) {
        *buf_off = 0;
        return fread(buffer, 1, BUF_SIZE, f);
    }
    return BUF_SIZE - *buf_off;
}

char _getc(char buf[BUF_SIZE], int *off, FILE *f) {
    if (*(buf + *off) == 0 || *off >= BUF_SIZE - 1) {
        *off = 1;
        long c = fread(buf + *off, 1, BUF_SIZE - *off, f);
        if (*off + c < BUF_SIZE)
            buf[*off + c] = 0;
        if (c == 0)
            return 0;
    }
    char res = buf[*off];
    (*off)++;
    return res;
}

int is_sep(char c) {
    return c == ',' || c == ';' || c == '(' || c == ')';
}

long tokenise(char **s, size_t *cap, char buf[BUF_SIZE], int *off, FILE *f) {
    if (*s == NULL) {
        *cap = 32;
        *s = malloc(*cap);
        if (*s == NULL)
            return 0;
    }
    size_t i = 0;
    char c;
    do {
        c = _getc(buf, off, f);
    } while (c && isspace(c));

    if (c == '%') {
        do {
            c = _getc(buf, off, f);
        } while (c && c != '\n');
        do {
            c = _getc(buf, off, f);
        } while (c && isspace(c));
    }

    if (c == '{') {
        do {
            c = _getc(buf, off, f);
        } while (c && c != '}');
        do {
            c = _getc(buf, off, f);
        } while (c && isspace(c));
    }

    if (!c) {
        (*s)[0] = 0;
        return 0;
    }

    bool num = isalnum(c);
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
        c = _getc(buf, off, f);
    } while (c && !isspace(c) && !is_sep(c) && isalnum(c) == num && c != '{' &&
             c != '%');
    (*off)--;

    (*s)[i] = 0;
    return i;
}

int main(int argc, const char *argw[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: input file not provided");
        fflush(stderr);
        return 1;
    }
    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[1]);
        fflush(stderr);
        return 1;
    }

    // size_t vars[VAR_CNT] = {0};
    char buf[BUF_SIZE] = {0};
    size_t tok_len = 0;
    int off = 0;
    char *tok = NULL;
    while (true) {
        int n = tokenise(&tok, &tok_len, buf, &off, f);
        if (n <= 0)
            break;
        printf("%d : [%s]\n", n, tok);
    }

    fclose(f);
    return 0;
}
