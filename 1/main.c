#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum Status {
    STATUS_OK = 0,
    STATUS_INVALID_INPUT = 1,
    STATUS_ALLOCATION_FAILURE = 2,
    STATUS_OVERFLOW = 3,
} Status;

typedef Status (*handle)(const char *str, int rest_c, const char *rest[]);

Status str_eq(bool *res, const char *a, const char *b) {
    while (*a == *b && *a != 0 && *b != 0) {
        a++;
        b++;
    }
    *res = *a == *b;
    return STATUS_OK;
}

Status str_len(int *res, const char *str) {
    *res = 0;
    while (*str) {
        str++;
        (*res)++;
    }
    return STATUS_OK;
}

Status parse_uint(unsigned int *res, const char *str) {
    *res = 0;
    unsigned int prev;
    while (*str) {
        if (!isnumber(*str))
            return STATUS_INVALID_INPUT;
        prev = *res;
        *res = (*res * 10) + (*str - '0');
        if (*res < prev)
            return STATUS_OVERFLOW;
        str++;
    }
    return STATUS_OK;
}

Status handle_l(const char *str, int rest_c, const char *rest[]) {
    (void)rest_c;
    (void)rest;

    int res;
    str_len(&res, str);
    printf("Length of string is %d\n", res);
    return STATUS_OK;
}

Status handle_r(const char *str, int rest_c, const char *rest[]) {
    (void)rest_c;
    (void)rest;

    int len;
    str_len(&len, str);
    char *new = malloc(len * sizeof(char));
    if (!new) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return STATUS_ALLOCATION_FAILURE;
    }
    for (int i = 0; i < len; i++) {
        new[i] = str[len - 1 - i];
    }
    printf("New string: \"%s\"\n", new);
    free(new);
    return STATUS_OK;
}

Status handle_u(const char *str, int rest_c, const char *rest[]) {
    (void)rest_c;
    (void)rest;

    int len;
    str_len(&len, str);
    char *new = malloc(len * sizeof(char));
    if (!new) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return STATUS_ALLOCATION_FAILURE;
    }
    for (int i = 0; i < len; i++) {
        if (i % 2 == 1)
            new[i] = toupper(str[i]);
        else
            new[i] = str[i];
    }
    printf("New string: \"%s\"\n", new);
    free(new);

    return STATUS_OK;
}

Status handle_n(const char *str, int rest_c, const char *rest[]) {
    (void)rest_c;
    (void)rest;

    int len;
    str_len(&len, str);
    char *new = malloc(len * sizeof(char));
    if (!new) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return STATUS_ALLOCATION_FAILURE;
    }
    int j = 0;
    for (int i = 0; i < len; i++) {
        if (isnumber(str[i])) {
            new[j] = str[i];
            j++;
        }
    }
    for (int i = 0; i < len; i++) {
        if (isalpha(str[i])) {
            new[j] = str[i];
            j++;
        }
    }
    for (int i = 0; i < len; i++) {
        if (!isalnum(str[i])) {
            new[j] = str[i];
            j++;
        }
    }

    printf("New string: \"%s\"\n", new);
    free(new);
    return 0;
}

Status handle_c(const char *str, int rest_c, const char *rest[]) {
    if (rest_c < 1) {
        fprintf(stderr, "ERROR: no seed provided\n");
        return STATUS_INVALID_INPUT;
    }
    unsigned int seed;
    int status = parse_uint(&seed, rest[0]);
    if (status == STATUS_OVERFLOW) {
        fprintf(stderr, "ERROR: seed is too big\n");
        return status;
    }
    if (status == STATUS_INVALID_INPUT) {
        fprintf(stderr, "ERROR: failed to parse \"%s\" as unsigned int\n", rest[0]);
        return status;
    }
    srand(seed);
    if (rest_c == 0) {
        printf("No strings to concatinate\n");
        return STATUS_OK;
    }
    rest[0] = str;

    int total_len = 0;
    int len;
    for (int i = 0; i < rest_c; i++) {
        str_len(&len, rest[i]);
        total_len += len;
    }

    char *new = malloc(total_len * sizeof(char));
    if (!new) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return STATUS_ALLOCATION_FAILURE;
    }
    bool *checks = malloc(rest_c * sizeof(bool));
    if (!checks) {
        free(new);
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return STATUS_ALLOCATION_FAILURE;
    }

    for (int i = 0; i < rest_c; i++)
        checks[i] = 0;

    int j = 0;
    int cnt = 0;
    while (cnt < rest_c) {
        int k = rand() % rest_c;
        if (checks[k])
            continue;
        str_len(&len, rest[k]);
        for (int i = 0; i < len; i++) {
            new[j] = rest[k][i];
            j++;
        }
        checks[k] = true;
        cnt++;
    }
    printf("New string: \"%s\"\n", new);

    free(new);
    free(checks);

    return STATUS_OK;
}

int main(int argc, const char *argw[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: Not enough args. Expected at least 2, got %d\n",
                argc - 1);
        return STATUS_INVALID_INPUT;
    }
    char *flags[] = {"-l", "-r", "-u", "-n", "-c"};
    handle handles[] = {handle_l, handle_r, handle_u, handle_n, handle_c};
    bool eq = false;
    for (int i = 0; i < (int)(sizeof(flags) / sizeof(flags[0])); i++) {
        str_eq(&eq, argw[1], flags[i]);
        if (eq) {
            return handles[i](argw[2], argc - 3, argw + 3);
        }
    }
    fprintf(stderr, "ERROR: unknown flag: %s\n", argw[1]);
    return STATUS_INVALID_INPUT;
}
