
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 128

int check_is_valid(int base, char c, bool *res) {
    if (c >= '0' && c <= '9' && c <= '0' + base - 1)
        *res = true;
    else if (c >= 'A' && c <= 'A' + base - 11)
        *res = true;
    else
        *res = false;
    return 0;
}

int parse(int base, const char *line, long *out) {
    *out = 0;
    bool neg = false;
    const char *c = line;
    bool is_valid = true;
    if (*c == '-') {
        neg = true;
        c++;
    }

    for (; *c != 0 && *c != '\n'; c++) {
        check_is_valid(base, *c, &is_valid);
        if (!is_valid)
            return 1;
        *out *= base;
        if ('0' <= *c && *c <= '9')
            *out += *c - '0';
        if ('A' <= *c)
            *out += *c - 'A' + 10;
    }
    if (neg)
        *out *= -1;

    return 0;
}

int print_in_base(int base, long val, char *out) {
    if (val == 0) {
        out[0] = '0';
        out[1] = 0;
        return 0;
    }
    if (val < 0) {
        out[0] = '-';
        out++;
        val *= -1;
    }
    int i = 0;
    while (val > 0) {
        int remainder = val % base;
        char c = remainder <= 9 ? '0' + remainder : 'A' + remainder - 10;
        out[i] = c;
        i++;
        val /= base;
    }
    for (int j = 0; j < i / 2; j++) {
        char c = out[j];
        out[j] = out[i - 1 - j];
        out[i - 1 - j] = c;
    }
    out[i] = 0;

    return 0;
}

int main(void) {
    int base;
    if (scanf("%d", &base) != 1) {
        fprintf(stderr, "ERROR: base must be a number\n");
        return 1;
    }
    if (base < 2 || base > 36) {
        fprintf(stderr, "ERROR: base must be in range [2..36]\n");
        return 1;
    }
    char line[MAX_SIZE] = {0};

    long max = 0;
    long cur = 0;
    while (fgets(line, MAX_SIZE, stdin)) {
        if (line[0] == '\n')
            continue;
        if (parse(base, line, &cur)) {
            line[strcspn(line, "\n")] = 0;
            fprintf(stderr, "ERROR: failed to parse \"%s\"\n", line);
        };
        if (labs(cur) > labs(max))
            max = cur;
    }
    print_in_base(10, max, line);
    printf("%s\n", line);
    print_in_base(9, max, line);
    printf("%s\n", line);
    print_in_base(18, max, line);
    printf("%s\n", line);
    print_in_base(27, max, line);
    printf("%s\n", line);
    print_in_base(36, max, line);
    printf("%s\n", line);
    return 0;
}
