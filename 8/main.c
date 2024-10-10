
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int r = MACRO;                                                         \
        if (r) {                                                               \
            AFTER;                                                             \
            return r;                                                          \
        }                                                                      \
    }

typedef enum Status {
    S_OK,
    S_INVALID_INPUT_ERROR,
    S_INVALID_BASE_ERROR,
    S_MALLOC_ERROR,
} Status;

int char_to_int(int *res, int base, char c) {
    int limit_0 = base < 10 ? base : 10;
    if (0 <= c && c < '0' + limit_0)
        *res = c - '0';
    else if ('A' <= c && c < 'A' + base - 10)
        *res = c - 'A' + 10;
    else if ('a' <= c && c < 'a' + base - 10)
        *res = c - 'a' + 10;
    else
        return S_INVALID_INPUT_ERROR;
    return S_OK;
}

int int_to_char(char *res, int base, int c) {
    if (c >= base)
        return S_INVALID_INPUT_ERROR;
    if (c < 10)
        *res = '0' + c;
    else
        *res = 'A' + c - 10;
    return S_OK;
}

int string_reverse(char *s, int n) {
    for (int i = 0; i < n / 2; i++) {
        char t = s[i];
        s[i] = s[n - i - 1];
        s[n - i - 1] = t;
    }
    return S_OK;
}

int add(char **res, int base, const char *a, const char *b) {
    if (base < 2 || base > 36)
        return S_INVALID_BASE_ERROR;

    if (strlen(a) < strlen(b)) {
        const char *t = a;
        a = b;
        b = t;
    }
    int len_a = strlen(a);
    int len_b = strlen(b);
    int len_c = len_a + 1;
    char *c = malloc(len_c);
    int carry = 0;
    if (!c)
        return S_MALLOC_ERROR;
    int i;
    for (i = 0; i < len_b; i++) {
        int a_d, b_d;
        check(char_to_int(&a_d, base, a[len_a - i - 1]), free(c));
        check(char_to_int(&b_d, base, b[len_b - i - 1]), free(c));
        int c_d = a_d + b_d + carry;
        carry = 0;
        if (c_d >= base) {
            c_d -= base;
            carry = 1;
        }
        check(int_to_char(&c[i], base, c_d), free(c));
    }
    for (; i < len_a; i++) {
        int a_d;
        check(char_to_int(&a_d, base, a[len_a - i - 1]), free(c));
        int c_d = a_d + carry;
        carry = 0;
        if (c_d >= base) {
            c_d -= base;
            carry = 1;
        }
        check(int_to_char(&c[i], base, c_d), free(c));
    }
    if (carry) {
        c[i] = '1';
        i++;
    }
    string_reverse(c, i);
    *res = c;
    return S_OK;
}

int sum(char **res, int base, int n, ...) {
    char *a;
    char *c;
    check(add(&a, base, "0", "0"), {});

    va_list valist;
    va_start(valist, n);
    for (int i = 0; i < n; i++) {
        char *b = va_arg(valist, char *);
        check(add(&c, base, a, b), {va_end(valist); free(a);});
        free(a);
        a = c;
    }
    va_end(valist);

    *res = c;
    return S_OK;
}

int main(void) {
    char *res;
    int r;
    r = sum(&res, 10, 6, "999999", "1", "4", "9", "128", "9");
    if (!r) {
        printf("%s\n", res);
        free(res);
    } else {
        printf("ERROR: %d\n", r);
    }

    // 100011111011001
    r = sum(&res, 2, 5, "10110", "111001", "100011101110111", "11", "10000");
    if (!r) {
        printf("%s\n", res);
        free(res);
    } else {
        printf("ERROR: %d\n", r);
    }

    // 33 + 33 + 38 + 35 + 10 + 1297 = 1448 = 146_36
    r = sum(&res, 36, 6, "X", "X", "12", "Z", "A", "101");
    if (!r) {
        printf("%s\n", res);
        free(res);
    } else {
        printf("ERROR: %d\n", r);
    }

    r = sum(&res, 34, 6, "X", "X", "12", "Z", "A", "101");
    if (!r) {
        printf("%s\n", res);
        free(res);
    } else {
        printf("ERROR: %d\n", r);
    }

    r = sum(&res, 40, 6, "X", "X", "12", "Z", "A", "101");
    if (!r) {
        printf("%s\n", res);
        free(res);
    } else {
        printf("ERROR: %d\n", r);
    }
}
