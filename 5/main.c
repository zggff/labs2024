#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*printer)(void *f, const char *s, ...);
typedef int (*vprinter)(void *f, const char *s, va_list valist);

int fprintf_wrapper(void *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = vfprintf(f, s, valist);
    va_end(valist);
    return res;
}

int sprintf_wrapper(void *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = vsprintf(f, s, valist);
    va_end(valist);
    return res;
}

int vfprintf_wrapper(void *f, const char *s, va_list valist) {
    return vfprintf(f, s, valist);
}

int vsprintf_wrapper(void *f, const char *s, va_list valist) {
    return vsprintf(f, s, valist);
}

int printf_general(void *f, printer p, vprinter vp, const char *s,
                   va_list valist) {
    int len = strlen(s) + 1;
    char *str = malloc(len);
    if (!str)
        return 1;
    memcpy(str, s, len);
    char *ptr = str;
    while (str) {
        char *prev = str;
        str = strchr(str + 1, '%');
        if (str)
            *str = 0;


        if (strncmp(prev, "%Ro", 3) == 0) {
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%Zr", 3) == 0) {
            va_arg(valist, unsigned int);
            prev += 3;
        }
        if (strncmp(prev, "%Cv", 3) == 0) {
            va_arg(valist, int);
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%CV", 3) == 0) {
            va_arg(valist, int);
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%to", 3) == 0) {
            va_arg(valist, char *);
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%TO", 3) == 0) {
            va_arg(valist, char *);
            va_arg(valist, int);
            prev += 3;
        }
        if (strncmp(prev, "%mi", 3) == 0) {
            va_arg(valist, int32_t);
            prev += 3;
        }
        if (strncmp(prev, "%mu", 3) == 0) {
            va_arg(valist, uint32_t);
            prev += 3;
        }
        if (strncmp(prev, "%md", 3) == 0) {
            va_arg(valist, double);
            prev += 3;
        }
        if (strncmp(prev, "%mf", 3) == 0) {
            va_arg(valist, double);
            prev += 3;
        }

        va_list copy;
        va_copy(copy, valist);
        vp(f, prev, copy);
        va_end(copy);

        if (prev[0] == '%') {
            for (; *prev != 0; prev++) {
                *prev = tolower(*prev);
                if (*prev == 'd' || *prev == 'u' || *prev == 'x' ||
                    *prev == 'o' || *prev == 'c') {
                    va_arg(valist, int);
                    break;
                }
                if (*prev == '*') {
                    va_arg(valist, int);
                    continue;
                }
                if (*prev == 'f') {
                    va_arg(valist, double);
                    break;
                }
                if (*prev == 'l') {
                    va_arg(valist, long);
                    break;
                }
                if (*prev == 'p' || *prev == 's') {
                    va_arg(valist, void *);
                    break;
                }
            }
        }
        if (str)
            str[0] = '%';
    }

    (void) p;

    free(ptr);
    return 0;
}

int overfprintf(FILE *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = printf_general(f, fprintf_wrapper, vfprintf_wrapper, s, valist);
    va_end(valist);
    return res;
}

int oversprintf(FILE *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = printf_general(f, fprintf_wrapper, vfprintf_wrapper, s, valist);
    va_end(valist);
    return res;
}

int main(void) {
    overfprintf(stdout, "hello, [%d] {%.*f} {%u} (%X)\n", 10, 2, 0.5, 125, 0xff);
}
