#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef enum Status {
    S_OK = 0,
    S_NOT_A_POLIGON = 1,
    S_NOT_A_NUMBER = 2,
    S_BUFFER_OVERFLOW = 3,
    S_OVERFLOW = 4,
} Status;

typedef struct Pos {
    float x;
    float y;
} Pos;

Status calc_z_product(float *res, Pos a, Pos b, Pos c) {
    float dx1 = b.x - c.x;
    float dy1 = b.y - c.y;
    float dx2 = a.x - b.x;
    float dy2 = a.y - b.y;
    *res = dx1 * dy2 - dy1 * dx2;
    return S_OK;
}

// note this only works for notintersecting polygons
Status is_convex(bool *res, ...) {
    *res = false;
    va_list valist;
    va_start(valist, res);
    Pos first = {0};
    Pos second = {0};
    Pos a = {0};
    Pos b = {0};
    Pos c = {0};
    int cnt = 0;
    int cnt_pos = 0;
    float z_cross;

    while (true) {
        c = b;
        b = a;
        a = va_arg(valist, Pos);

        if (isnan(a.x) || isnan(a.y))
            break;

        cnt++;
        if (cnt == 1)
            first = a;
        if (cnt == 2)
            second = a;
        if (cnt < 3)
            continue;

        calc_z_product(&z_cross, a, b, c);
        if (z_cross > 0)
            cnt_pos++;
    }
    va_end(valist);

    if (cnt < 3) {
        return S_NOT_A_POLIGON;
    }

    calc_z_product(&z_cross, first, b, c);
    if (z_cross > 0)
        cnt_pos++;

    calc_z_product(&z_cross, second, first, b);
    if (z_cross > 0)
        cnt_pos++;

    *res = cnt_pos == 0 || cnt_pos == cnt;

    return S_OK;
}

int pow_custom(double *res, double x, int n) {
    bool rev = n < 0;
    if (rev)
        n *= -1;
    *res = 1;
    for (;;) {
        if (n & 1)
            (*res) *= x;
        n >>= 1;
        if (!n)
            break;
        x *= x;
    }
    if (rev)
        *res = 1 / *res;
    return 0;
}

Status polynomial(double *res, double x, int n, ...) {
    va_list valist;
    va_start(valist, n);
    *res = 0;
    for (int i = n; i >= 0; i--) {
        double k = va_arg(valist, double);
        double xp;
        pow_custom(&xp, x, i);
        *res += k * xp;
        // *res += k * pow(x, i);
    }
    va_end(valist);
    return S_OK;
}

Status parse_digit(int *res, int base, char c) {
    c = tolower(c);
    if ('0' <= c && c <= '0' + base - 1 && c <= '9') {
        *res = c - '0';
        return S_OK;
    }
    if ('a' <= c && c <= 'a' + base - 11 && c <= 'z') {
        *res = c - 'a' + 10;
        return S_OK;
    }
    return S_NOT_A_NUMBER;
}

Status parse_uint_n(unsigned int *res, int base, const char *str, int n) {
    *res = 0;
    int digit;
    for (int i = 0; i < n; i++) {
        if (str[i] == 0)
            return S_BUFFER_OVERFLOW;
        if (parse_digit(&digit, base, str[i]) != S_OK)
            return S_NOT_A_NUMBER;
        unsigned int prev = *res;
        *res = (*res * base) + digit;
        if (*res < prev)
            return S_OVERFLOW;
    }
    return S_OK;
}

Status parse_uint(unsigned int *res, int base, const char *str) {
    int n = strlen(str);
    return parse_uint_n(res, base, str, n);
}

Status write_uint(char *str, int *n, int base, unsigned int num) {
    int i = 0;
    while (num > 0) {
        char digit = num % base;
        digit = digit < 10 ? '0' + digit : 'A' + digit - 10;
        str[i] = digit;
        num /= base;
        i++;
    }
    str[i] = 0;
    for (int j = 0; j < i / 2; j++) {
        char tmp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = tmp;
    }
    *n = i;
    return S_OK;
}

Status check_kaprekars(bool *res, int base, const char *str) {
    unsigned int num, a, b, sqr;
    char buffer[64];
    int len;

    *res = false;
    if (parse_uint(&num, base, str) != S_OK) {
        return S_NOT_A_NUMBER;
    }

    sqr = num * num;
    write_uint(buffer, &len, base, sqr);
    // printf("\n%s\n", buffer);
    for (int i = 1; i <= len - 1; i++) {
        parse_uint_n(&a, base, buffer, i);
        parse_uint_n(&b, base, buffer + i, len - i);
        // printf("%d + %d = %d [%d]\n", a, b, a + b, num);
        if (a != 0 && b != 0 && a + b == num) {
            *res = true;
            return S_OK;
        }
    }
    return S_OK;
}

Status find_kaprekars(unsigned int base, ...) {
    va_list valist;

    va_start(valist, base);
    bool is_kaprekar;
    while (1) {
        const char *number = va_arg(valist, char *);
        if (number == NULL) {
            break;
        }
        if (check_kaprekars(&is_kaprekar, base, number) != S_OK)
            return S_NOT_A_NUMBER;
        if (is_kaprekar) {
            printf("%s\n", number);
        }
    }
    va_end(valist);
    return S_OK;
}

#ifndef MODE
#define MODE 2
#endif

#if MODE == 1
int main(void) {
    bool res;
    Pos a = {0, 0};
    Pos b = {0, 4};
    Pos c = {2, 0};
    Pos d = {4, 4};
    Pos e = {4, 2};
    Pos f = {4, 0};
    int r = is_convex(&res, a, b, c, d, e, f, NAN);
    printf("%d\t%d\n", r, res);

    return S_OK;
}
#else
#if MODE == 2
int main(void) {
    double res = 0;
    polynomial(&res, 0.5, 5, 12.1, -8.2, 12.4, -0.2, 0.5, 2.4);
    printf("%f\n", res);
    return S_OK;
}
#else
#if MODE == 3
int main(void) {
    // return find_kaprekars(7, "3", "4", "6", "10", NULL);
    // return find_kaprekars(8, "34", "44", "77", "100", NULL);
    // return find_kaprekars(15, "7", "8", "E", "10", NULL);
    return find_kaprekars(10, "9", "45", "55", "99", "297", "703", "999",
                          "2223", "2728", "4879", "4950", "5050", "7272",
                          "7777", "7778", NULL);
}
#else
int main(void) {
    printf("unknown mode\n");
    return S_OK;
}
#endif
#endif
#endif
