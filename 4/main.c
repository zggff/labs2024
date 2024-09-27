#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEN 64

typedef int (*handle)(FILE *in, FILE *out);

int handle_d(FILE *in, FILE *out) {
    char c;
    while (true) {
        c = fgetc(in);
        if ('0' <= c && c <= '9')
            continue;
        if (c == EOF)
            break;
        fputc(c, out);
    }
    return 0;
}

int handle_i(FILE *in, FILE *out) {
    char c;
    int count = 0;
    do {
        c = fgetc(in);
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            count++;
        if (c == '\n') {
            fprintf(out, "%d\n", count);
            count = 0;
        }
    } while (c != EOF);
    return 0;
}

int handle_s(FILE *in, FILE *out) {
    char c;
    int count = 0;
    do {
        c = fgetc(in);
        if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
              ('0' <= c && c <= '9') || (c == ' ') || (c=='\n')))
            count++;

        if (c == '\n') {
            fprintf(out, "%d\n", count);
            count = 0;
        }
    } while (c != EOF);
    return 0;
}

int handle_a(FILE *in, FILE *out) {
    char c;
    while(true) {
        c = fgetc(in);
        if ('0' <= c && c <= '9') {
            fputc(c, out);
            continue;
        }
        if (c == EOF)
            break;
        fprintf(out, ":%x:", c);
    } ;
    return 0;
}

int find_and_execute_subprogram(const char *arg, int offset, const char flags[],
                                const handle handles[], FILE *in, FILE *out) {
    for (int i = 0; i < 4; i++) {
        if (arg[1 + offset] == flags[i]) {
            int res = handles[i](in, out);
            if (res != 0) {
                return res;
            }
            return 0;
        }
    }
    fprintf(stderr, "ERROR: unknown flag\n");
    return -1;
}

int main(int argc, const char *argw[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: not enough arguments provided\n");
        return 1;
    }
    if (argw[1][0] != '-' && argw[1][0] != '/') {
        fprintf(stderr, "ERROR: first argument must be a flag\n");
        return 1;
    }
    char in_file_name[MAX_LEN];
    char out_file_name[MAX_LEN];
    int offset = 0;

    strcpy(in_file_name, argw[2]);
    if (argw[1][1] == 'n') {
        if (argc < 4) {
            fprintf(stderr, "ERROR: not enough arguments provided\n");
            return 1;
        }
        offset++;
        strcpy(out_file_name, argw[3]);
    } else {
        strcpy(out_file_name, "out_");
        strcpy(out_file_name + 4, in_file_name);
    }

    FILE *in = fopen(in_file_name, "r");
    FILE *out = fopen(out_file_name, "w");
    if (!in || !out) {
        fprintf(stderr, "ERROR: failed to open files\n");
        if (in)
            fclose(in);
        if (out)
            fclose(out);
        return 1;
    }

    char flags[] = {'d', 'i', 's', 'a'};
    handle handles[] = {handle_d, handle_i, handle_s, handle_a};

    int res =
        find_and_execute_subprogram(argw[1], offset, flags, handles, in, out);

    fclose(in);
    fclose(out);
    return res;
}
