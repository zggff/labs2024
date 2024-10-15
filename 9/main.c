#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

int main(int argc, const char *argw[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: input file not provided\n");
        return 1;
    }
    if (argc < 3) {
        fprintf(stderr, "ERROR: at least one separator must be provided\n");
        return 1;
    }
    char *sep = malloc(argc - 1);
    for (int i = 2; i < argc; i++) {
        if (argw[i][0] == '\\' && strlen(argw[i]) == 2) {
            switch (argw[i][1]) {
            case 't':
                sep[i - 2] = '\t';
                break;
            case 'n':
                sep[i - 2] = '\n';
                break;
            case '\\':
                sep[i - 2] = '\\';
                break;
            case '\'':
                sep[i - 2] = '\'';
                break;
            case '"':
                sep[i - 2] = '"';
                break;
            default:
                fprintf(stderr, "ERROR: unsupported escape sequence: [%s]\n",
                        argw[i]);
                return 1;
            }
            continue;
        }
        if (strlen(argw[i]) > 1) {
            fprintf(stderr, "ERROR: each separator must be a signle symbol\n");
            return 1;
        }
        sep[i - 2] = argw[i][0];
    }
    sep[argc - 1] = 0;

    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file [%s]\n", argw[1]);
        free(sep);
        return 1;
    }

    Tree t = {0};
    Status s = tree_parse_file(&t, sep, f);
    fclose(f);
    free(sep);

    tree_print(&t);
    char *line = NULL;
    size_t line_len = 0;
    while (true) {
        int n = getline(&line, &line_len, stdin);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;

        const char *s = line;
        char *op;
        parse_field_str(&op, &s, isspace);
        printf("[%s]\n", op);
    }

    tree_free(&t);
    return s;
}
