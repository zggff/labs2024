#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
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

int print_roman(void *f, printer p, int n) {
    if (n == 0)
        return p(f, "0");

    if (n < 0) {
        n *= -1;
        p(f, "-");
    }

    int ns[] = {1, 4, 5, 9, 10, 40, 50, 90, 100, 400, 500, 900, 1000};
    char *s[] = {"I",  "IV", "V",  "IX", "X",  "XL", "L",
                 "XC", "C",  "CD", "D",  "CM", "M"};
    for (int i = sizeof(s) / sizeof(s[0]) - 1; i >= 0; i--) {
        int cnt = n / ns[i];
        n = n % ns[i];
        for (int j = 0; j < cnt; j++) {
            p(f, "%s", s[i]);
        }
    }
    return 0;
}

int print_zeckendorf(void *f, printer p, unsigned int n) {
    // 1 2 3 5 8 13 21 34 55 89
    // 0 0 1 0 1 0  0  0  0  1  1
    // 89 + 8 + 3
    int size = 16;
    unsigned int *fib = malloc(size * sizeof(unsigned int));
    if (!fib)
        return 1;
    fib[0] = 1;
    fib[1] = 2;
    int i = 2;
    while (1) {
        if (fib[i - 2] + fib[i - 1] > n)
            break;
        if (i >= size) {
            size *= 2;
            unsigned int *tmp = realloc(fib, size * sizeof(unsigned int));
            if (!tmp) {
                free(fib);
                return 1;
            }
            fib = tmp;
        }
        fib[i] = fib[i - 2] + fib[i - 1];
        i++;
    }
    for (int j = i - 1; j >= 0; j--) {
        if (n >= fib[j]) {
            n -= fib[j];
            fib[j] = 1;
        } else {
            fib[j] = 0;
        }
    }
    for (int j = 0; j < i; j++)
        p(f, "%d", fib[j]);
    p(f, "1");
    free(fib);
    return 0;
}

int print_in_base(void *f, printer p, int base, int val, char start) {
    if (val == 0)
        return p(f, "0");

    if (base < 2 || base > 36)
        base = 10;

    if (val < 0) {
        p(f, "-");
        val *= -1;
    }

    int i = 0;
    char str[128];
    while (val > 0) {
        int remainder = val % base;
        char c = remainder <= 9 ? '0' + remainder : start + remainder - 10;
        str[i] = c;
        val /= base;
        i++;
    }
    for (int j = 0; j < i / 2; j++) {
        char c = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = c;
    }
    str[i] = 0;
    p(f, "%s", str);
    return 0;
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
            int n = va_arg(valist, int);
            print_roman(f, p, n);
            prev += 3;
        }
        if (strncmp(prev, "%Zr", 3) == 0) {
            unsigned int n = va_arg(valist, unsigned int);
            print_zeckendorf(f, p, n);
            prev += 3;
        }
        if (strncmp(prev, "%Cv", 3) == 0) {
            int n = va_arg(valist, int);
            int base = va_arg(valist, int);
            print_in_base(f, p, base, n, 'a');
            prev += 3;
        }
        if (strncmp(prev, "%CV", 3) == 0) {
            int n = va_arg(valist, int);
            int base = va_arg(valist, int);
            print_in_base(f, p, base, n, 'A');
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
    overfprintf(stdout, "hello, [%d] {%.*f} {%u} (%X)\n", 10, 2, 0.5, 125,
                0xff);
    overfprintf(stdout, "[%Ro] [%Ro] [%Ro]\n", 3549, -49, 191);
    overfprintf(stdout, "[%Zr] [%Zr] [%Zr]\n", 100, 100, 100);
    overfprintf(stdout, "[%Cv] [%CV] [%CV] [%CV]\n", 0xffe1, 16, 8, 2, 1293, 36, -0xabcd12, 16);
}
