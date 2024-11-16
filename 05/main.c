#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Status { S_OK, S_INVALID_INPUT, S_MALLOC, S_UNIMPLEMENTED } Status;

int calculate(const char *line, const char *fname, size_t index_in_file) {
    (void)line;
    (void)fname;
    fprintf(stderr, "ERROR: unimplemented");
    exit(-2);
}

int table(const char *line, const char *fname) {
    (void)line;
    (void)fname;
    fprintf(stderr, "ERROR: unimplemented");
    exit(-2);
}

int main(int argc, char *argw[]) {
    int mode = 0;
    int skip = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argw[i], "--calculate") == 0) {
            if (mode != 0) {
                fprintf(stderr, "ERROR: only one mode flag can be provided\n");
                fflush(stderr);
                return 1;
            }
            mode = 1;
            skip = i;
        } else if (strcmp(argw[i], "--table") == 0) {
            if (mode != 0) {
                fprintf(stderr, "ERROR: only one mode flag can be provided\n");
                fflush(stderr);
                return 1;
            }
            mode = 2;
            skip = i;
        }
    }
    if (mode == 0) {
        fprintf(stderr, "ERROR: mode flag not provided\n");
        fflush(stderr);
        return 1;
    }
    if (argc < 3) {
        fprintf(stderr, "ERROR: input file not provided\n");
        fflush(stderr);
        return 1;
    }
    size_t line_len = 0;
    char *line = NULL;

    int res = 0;
    for (int i = 1; !res && i < argc; i++) {
        if (i == skip)
            continue;
        FILE *f = fopen(argw[i], "r");
        if (!f) {
            fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[i]);
            fflush(stderr);
            return 2;
        }
        size_t index_in_file = 0;
        while (!res) {
            int n = getline(&line, &line_len, f);
            if (n <= 0)
                break;
            n--;
            line[n] = 0;
            if (mode == 1) {
                res = calculate(line, argw[i], index_in_file);
            } else {
                res = table(line, argw[i]);
            }
            index_in_file++;
        }
        fclose(f);
    }
    if (line)
        free(line);
    return 0;
}
