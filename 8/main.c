#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef long long ll;

#define STR_LEN 63 // long long is 64 bits - 1 bit for sign

int convert_hex_digit_to_decimal(char c, int *res) {
    *res = c <= '9' ? c - '0' : c - 'A' + 10;
    return 0;
}

int read_from_file(FILE *in, char *buffer, ll *number, int *base, bool *done) {
    char c;
    int i = 0;
    int digit = 0;
    bool neg = false;
    bool started = false;
    bool is_num = false;
    *number = 0;
    *base = 1;

    while (true) {
        if (i > STR_LEN) {
            buffer[i] = 0;
            fprintf(stderr,
                    "ERROR: overflow \"%s\". Number can't be longer than %d "
                    "symbols\n",
                    buffer, STR_LEN);
            return 1;
        }
        c = fgetc(in);
        if (c == EOF) {
            *done = true;
            break;
        }
        if (isspace(c)) {
            break;
        }
        if (c == '-' && !neg && !started) {
            buffer[i++] = '-';
            neg = true;
            continue;
        }

        if (c == '0' && !started) { // remove leading zeros
            is_num = true;
            continue;
        }

        if (!isalnum(c)) {
            buffer[i++] = c; // 8
            buffer[i] = 0;   // 9
            fprintf(stderr, "ERROR: failed to parse number \"%s...\"\n",
                    buffer);
            return 1;
        }
        started = true;
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
    if (is_num && i == 0)
        buffer[i++] = '0';
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
    char buffer[STR_LEN + 2]; // + 1 termination + 1 for possible mistake
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
