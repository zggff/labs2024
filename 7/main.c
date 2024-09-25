#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef int (*handle)(int argc, const char *argv[]);

typedef int (*writer)(char c, FILE *out);

int copy_lexema(FILE *in, FILE *out, writer writer, bool *ended) {
    char c = fgetc(in);
    while (isspace(c)) {
        c = fgetc(in);
    }
    while (true) {
        if (c == EOF) {
            *ended = true;
            return 0;
        }
        if (isspace(c)) {
            fputc(' ', out);
            return 0;
        }
        writer(c, out);
        c = fgetc(in);
    }
}

int write_unchanged(char c, FILE *out) {
    fputc(c, out);
    return 0;
}

int write_lower_base4(char c, FILE *out) {
    if (isalpha(c)) {
        c = tolower(c);
    }
    int digit = 0;
    int l = (int)log2(c) & ~(1);
    putc(':', out);
    for (int i = l; i >= 0; i -= 2) {
        digit = (c & (1 << i | 1 << (i + 1))) >> i;
        putc('0' + digit, out);
    }
    putc(':', out);
    (void)out;
    return 0;
}

int write_lower(char c, FILE *out) {
    if (!isalpha(c)) {
        fputc(c, out);
    }
    fputc(tolower(c), out);
    return 0;
}

int write_base8(char c, FILE *out) {
    fprintf(out, ";%o;", c);
    return 0;
}

int hanle_r(int argc, const char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: filenames not provided\n");
        return 1;
    }
    FILE *in[2] = {fopen(argv[0], "r"), fopen(argv[1], "r")};
    FILE *out = fopen(argv[2], "w");
    if (!in[0] || !in[1] || !out) {
        fprintf(stderr, "ERROR: failed to open files\n");
        if (in[0])
            fclose(in[0]);
        if (in[1])
            fclose(in[1]);
        if (out)
            fclose(out);
        return 1;
    }
    bool ended[2] = {false, false};
    while (!ended[0] || !ended[1]) {
        for (int i = 0; i < 2; i++) {
            if (ended[i])
                continue;
            copy_lexema(in[i], out, write_unchanged, &ended[i]);
        }
    }
    fclose(in[0]);
    fclose(in[1]);
    fclose(out);
    return 0;
}

int hanle_a(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: filenames not provided\n");
        return 1;
    }
    FILE *in = fopen(argv[0], "r");
    FILE *out = fopen(argv[1], "w");
    if (!in || !out) {
        fprintf(stderr, "ERROR: failed to open files\n");
        if (in)
            fclose(in);
        if (out)
            fclose(out);
        return 1;
    }
    bool ended = false;
    int i = 1;
    while (!ended) {
        if (i % 10 == 0) {
            copy_lexema(in, out, write_lower_base4, &ended);
        } else if (i % 2 == 0) {
            copy_lexema(in, out, write_lower, &ended);
        } else if (i % 5 == 0) {
            copy_lexema(in, out, write_base8, &ended);
        } else {
            copy_lexema(in, out, write_unchanged, &ended);
        }
        i++;
    }
    fclose(in);
    fclose(out);
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: flag must be provided\n");
        return 1;
    }
    const char *flags[] = {"-r", "/r", "-a", "/a"};
    handle handles[] = {hanle_r, hanle_a};
    for (int i = 0; i < (int)(sizeof(flags) / sizeof(flags[0])); i++) {
        if (strcmp(argv[1], flags[i]) == 0) {
            return handles[i / 2](argc - 2, argv + 2);
        }
    }
    fprintf(stderr, "ERROR: unknown flag: \"%s\"", argv[1]);
    return 1;
}
