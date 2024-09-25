#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef long long ll;

int convert_hex_digit_to_decimal(char c, int *res) {
    *res = c <= '9' ? c - '0' : c - 'A' + 10;
    return 0;
}

int read_from_file(FILE *in, char *buffer, ll *number, int *base, bool *done) {
    char c;
    int i = 0;
    int digit = 0;
    bool neg = false;
    *number = 0;
    *base = 0;

    while (true) {
        c = fgetc(in);
        if (c == EOF) {
            *done = true;
            break;
        }
        if (isspace(c)) {
            break;
        }
        if (c == '-' && !neg) {
            buffer[i++] = '-';
            neg = true;
            continue;
        }

        if (!isalnum(c)) {
            buffer[i++] = c;
            buffer[i] = 0;
            fprintf(stderr, "ERROR: failed to parse number \"%s...\"\n", buffer);
            return 1;
        }
        if (isalpha(c)) {
            buffer[i] = toupper(c);
        } else {
            buffer[i] = c;
        }
        convert_hex_digit_to_decimal(buffer[i], &digit);
        i++;
        *base = digit > *base ? digit : *base;
    }
    (*base)++;
    buffer[i] = 0;
    for (int j = neg; j < i; j++) {
        convert_hex_digit_to_decimal(buffer[j], &digit);
        if (*number > LLONG_MAX / *base ||
            *number * *base > LLONG_MAX - digit) {
            fprintf(stderr,
                    "ERROR: overflow while parsing \"%s\" with base %d\n",
                    buffer, *base);
            return 1;
        }
        *number *= *base;
        *number += digit;
    }
    if (neg)
        *number *= -1;
    return 0;
}

int write_to_file(FILE *out, const char *buffer, ll number, int base) {
    if (buffer[0] == 0) {
        return 0;
    }
    fprintf(out, "%s\t%d\t%lld\n", buffer, base, number);
    return 0;
}

int main(int argc, const char *argw[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: not enough arguments provided\n");
        return 1;
    }

    FILE *in = fopen(argw[1], "r");
    FILE *out = fopen(argw[2], "w");
    if (!in || !out) {
        fprintf(stderr, "ERROR: failed to open file\n");
        if (in)
            fclose(in);
        if (out)
            fclose(out);
        return 1;
    }

    bool is_done = false;
    ll number;
    int base;
    char buffer[128];
    int res = 0;
    do {
        res = read_from_file(in, buffer, &number, &base, &is_done);
        if (res) {
            break;
        }
        res = write_to_file(out, buffer, number, base);
        if (res)
            break;
    } while (!is_done);

    fclose(in);
    fclose(out);
    return res;
}
