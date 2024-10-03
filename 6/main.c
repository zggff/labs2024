#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int count_chars(int *res, const char *s, char c) {
    for (; *s != 0; s++)
        (*res) += *s == c;
    return 0;
}

int advance_string(void **f, const char *s) {
    int len = strlen(s);
    int cnt;
    count_chars(&cnt, s, '%');
    char *s2 = malloc(len + cnt + 3);
    if (!s2)
        return -1;
    int j = 0;
    for (int i = 0; i < len; i++) {
        if (s[i] == '%') {
            s2[j] = '%';
            j++;
            s2[j] = '*';
            j++;
            continue;
        }
        s2[j] = s[i];
        j++;
    }
    s2[j] = '%';
    j++;
    s2[j] = 'n';
    j++;
    s2[j] = 0;
    int n = 0;
    sscanf(*f, s2, &n);

    if (n >= 0) {
        char *c = *f;
        c += n;
        *f = c;
    }
    free(s2);
    return 0;
}

typedef int (*scanner)(void **f, const char *s, ...);
typedef int (*vscanner)(void **f, const char *s, va_list valist);

int fscanf_wrapper(void **f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = vfscanf(*f, s, valist);
    va_end(valist);
    return res;
}

int sscanf_wrapper(void **f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = vsscanf(*f, s, valist);
    advance_string(f, s);
    va_end(valist);
    return res;
}

int vfscanf_wrapper(void **f, const char *s, va_list valist) {
    return vfscanf(*f, s, valist);
}

int vsscanf_wrapper(void **f, const char *s, va_list valist) {
    int res = vsscanf(*f, s, valist);
    advance_string(f, s);
    return res;
}

int scanf_general(void **f, scanner scan, vscanner vscan, const char *s,
                  va_list valist) {
    int len = strlen(s) + 1;
    char *str = malloc(len);
    if (!str)
        return 1;
    memcpy(str, s, len);
    char *ptr = str;
    char flags[] = {'c', 's', '[', 'd', 'i', 'u', 'o', 'x', 'X', 'n',
                    'a', 'A', 'e', 'E', 'f', 'F', 'g', 'G', 'p'};
    int flags_cnt = sizeof(flags) / sizeof(flags[0]);

    while (str) {
        char *prev = str;
        str = strchr(str + 1, '%');
        if (str)
            *str = 0;

        if (strncmp(prev, "%Ro", 3) == 0) {
            va_arg(valist, int *);
            prev += 3;
        }
        if (strncmp(prev, "%Zr", 3) == 0) {
            va_arg(valist, unsigned int *);
            prev += 3;
        }
        if (strncmp(prev, "%Cv", 3) == 0) {
            va_arg(valist, int *);
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%CV", 3) == 0) {
            va_arg(valist, int *);
            va_arg(valist, int);
            prev += 3;
        }

        va_list copy;
        va_copy(copy, valist);
        vscan(f, prev, copy);
        va_end(copy);

        if (prev[0] == '%') {
            for (; *prev != 0; prev++) {
                if (*prev == '*') {
                    break;
                }
                for (int i = 0; i < flags_cnt; i++) {
                    if (*prev == flags[i]) {
                        va_arg(valist, void *);
                        break;
                    }
                }
            }
        }
        if (str)
            str[0] = '%';
    }

    free(ptr);
    return 0;
}

int overfscanf(FILE *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res =
        scanf_general((void *)&f, fscanf_wrapper, vfscanf_wrapper, s, valist);
    va_end(valist);
    return res;
}

int oversscanf(char *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res =
        scanf_general((void *)&f, sscanf_wrapper, vsscanf_wrapper, s, valist);
    va_end(valist);
    return res;
}

int main(void) {
    char s[] = "13, 24";
    int a, b;
    oversscanf(s, "%d, %d", &a, &b);
    // sscanf(s, "%*d%n", &n);
    printf("[%d] [%d]\n", a, b);
    return 0;
}
