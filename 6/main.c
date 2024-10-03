#include <ctype.h>
#include <stdbool.h>
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

    j += 2;
    int n = 0;
    do {
        s2[j - 2] = '%';
        s2[j - 1] = 'n';
        s2[j] = 0;
        sscanf(*f, s2, &n);
        j--;
    } while (n == 0 && j >= 2);

    char *c = *f;
    c += n;
    *f = c;
    free(s2);
    return 0;
}

typedef int (*scanner)(void **f, const char *s, ...);
typedef int (*vscanner)(void **f, const char *s, va_list valist);
typedef int (*seeker)(void **f);

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

int fseek_wrapper(void **f) {
    fseek(*f, -1, SEEK_CUR);
    return 0;
}

int sseek_wrapper(void **f) {
    char *c = *f;
    c--;
    *f = c;
    return 0;
}

int scan_roman(void **f, scanner scan, seeker seek, int *n) {
    return 0;
}

int scan_zeckendorf(void **f, scanner scan, seeker seek, unsigned int *n) {
    return 0;
}

int parse_digit(int *digit, int base, char c, char start) {
    if ('0' <= c && c <= '0' + base - 1) {
        *digit = c - '0';
        return 0;
    }
    if (start <= c && c <= start + base - 11) {
        *digit = c - start + 10;
        return 0;
    }
    return 1;
}

int scan_base(void **f, scanner scan, seeker seek, int *n, int base,
              char start) {
    if (base < 2 || base > 36)
        base = 10;

    char c;
    scan(f, "%c", &c);
    bool neg = c == '-';
    if (neg)
        scan(f, "%c", &c);

    int digit = 0;
    int num = 0;
    while (c) {
        if (parse_digit(&digit, base, c, start)) {
            seek(f);
            break;
        }
        num = num * base + digit;
        scan(f, "%c", &c);
    }
    num = neg ? -num : num;
    *n = num;
    return 0;
}

int scanf_general(void **f, scanner scan, vscanner vscan, seeker seek,
                  const char *s, va_list valist) {
    int len = strlen(s) + 1;
    char *str = malloc(len);
    if (!str)
        return -1;
    memcpy(str, s, len);
    char *ptr = str;
    char flags[] = {'c', 's', '[', 'd', 'i', 'u', 'o', 'x', 'X', 'n',
                    'a', 'A', 'e', 'E', 'f', 'F', 'g', 'G', 'p'};
    int flags_cnt = sizeof(flags) / sizeof(flags[0]);
    int cnt = 0;

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
            int *n = va_arg(valist, int *);
            int base = va_arg(valist, int);
            scan_base(f, scan, seek, n, base, 'a');
            prev += 3;
        }
        if (strncmp(prev, "%CV", 3) == 0) {
            int *n = va_arg(valist, int *);
            int base = va_arg(valist, int);
            scan_base(f, scan, seek, n, base, 'A');
            prev += 3;
        }

        va_list copy;
        va_copy(copy, valist);
        int res = vscan(f, prev, copy);
        va_end(copy);

        if (prev[0] == '%' && prev[1] != '%' && prev[1] != '*' && res < 1)
            break;

        cnt++;

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
    return cnt;
}

int overfscanf(FILE *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = scanf_general((void *)&f, fscanf_wrapper, vfscanf_wrapper,
                            fseek_wrapper, s, valist);
    va_end(valist);
    return res;
}

int oversscanf(char *f, const char *s, ...) {
    va_list valist;
    va_start(valist, s);
    int res = scanf_general((void *)&f, sscanf_wrapper, vsscanf_wrapper,
                            sseek_wrapper, s, valist);
    va_end(valist);
    return res;
}

int main(void) {
    char *s = "13, 24";
    int a, b, c;
    float f = 0;
    int r = 0;
    // oversscanf(s, "%d, %d : %f", &a, &b, &f);
    // printf("%d %d %f\n", a, b, f);

    a = 0;
    b = 0;
    c = 0;
    f = 0;
    s = "ffe1, 100, -ZZ : 0.1";
    // oversscanf(s, "%Cv", &a, 16, &b, 2, &c, 36, &f);
    r = oversscanf(s, "%Cv, %Cv, %CV : %f", &a, 16, &b, 2, &c, 36, &f);
    printf("%d\t%d %d %d %f\n", r, a, b, c, f);
    return 0;
}
