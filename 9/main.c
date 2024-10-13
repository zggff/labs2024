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
        return 1;
    }

    char *word = NULL;
    size_t word_cap = 0;
    printf("(%s)\n", sep);
    while (true) {
        int n = getword(&word, &word_cap, f, sep);
        if (n <= 0)
            break;
        n--;
        word[n] = 0;
        if (n == 0)
            continue;
        printf("[%s]\n", word);
    }
    free(word);
    free(sep);
    return 0;
}
