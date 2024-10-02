#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum STATUS {
    S_OK = 0,
    S_FILE_OPEN_ERROR = 1,
    S_FILE_READ_ERROR = 2,
    S_MALLOC_ERROR = 3,
} STATUS;

STATUS str_str(int *symbol, int *line, char **str, const char *sub) {
    if (*sub == 0)
        return S_OK;

    const char *sub2 = sub;
    for (; **str != 0; (*str)++) {
        (*symbol)++;
        if (**str == '\n') {
            (*line)++;
            *symbol = 0;
        }
        if (**str != *sub)
            continue;

        char *str2 = *str;
        while (true) {
            if (*sub2 == 0) {
                return S_OK;
            }
            if (*sub2 != *str2)
                break;
            sub2++;
            str2++;
        }
        sub2 = sub;
    }
    (void)symbol;
    (void)line;

    *str = NULL;
    return S_OK;
}

STATUS find_all_in_file(const char *sub, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", fname);
        return S_FILE_OPEN_ERROR;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *str = malloc((fsize + 1) * sizeof(char));
    if (!str) {
        fclose(f);
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return S_MALLOC_ERROR;
    }
    int r = fread(str, fsize, 1, f);
    if (r < 1) {
        fclose(f);
        fprintf(stderr, "ERROR: failed to read file\n");
        return S_FILE_READ_ERROR;
    }
    str[fsize] = 0;
    fclose(f);

    int symbol = 0;
    int line = 1;

    char *next_segment = str;
    printf("\"%s\":\n", fname);
    while (1) {
        str_str(&symbol, &line, &next_segment, sub);
        if (!next_segment)
            break;
        next_segment++;
        printf("\t%d:%d\n", line, symbol);
    }

    free(str);
    return S_OK;
}

STATUS find_all(const char *str, ...) {
    va_list valist;
    va_start(valist, str);
    while (1) {
        const char *fname = va_arg(valist, char *);
        if (fname == NULL) {
            break;
        }
        int status = find_all_in_file(str, fname);
        if (status != S_OK)
            return status;
    }

    va_end(valist);
    return S_OK;
}

int main(void) {
    return find_all("return f", "makefile", "main.c", NULL);
}
