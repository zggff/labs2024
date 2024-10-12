#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Status {
    S_OK = 0,
    S_MALLOC_ERROR,
    S_WRITE_ERROR,
    S_PARSE_NUMBER_ERROR,
    S_PARSE_NOT_ENOUGH_FIELDS_ERROR,
    S_PARSE_TOO_MANY_FIELDS_ERROR,
} Status;

typedef struct Employee {
    unsigned long id;
    char *first_name;
    char *last_name;
    float pay;
} Employee;

typedef int (*cmp)(const void *a, const void *b);

int cmp_asc(const void *a0, const void *b0) {
    const Employee *a = a0;
    const Employee *b = b0;
    if (a->pay != b->pay)
        return a->pay < b->pay ? -1 : 1;
    int r = strcmp(a->last_name, b->last_name);
    if (r)
        return r;
    r = strcmp(a->first_name, b->first_name);
    if (r)
        return r;
    if (a->id < b->id)
        return a->id < b->id ? -1 : 1;
    return 0;
}

int cmp_des(const void *a0, const void *b0) {
    const Employee *a = a0;
    const Employee *b = b0;
    if (a->pay != b->pay)
        return a->pay > b->pay ? -1 : 1;
    int r = strcmp(a->last_name, b->last_name);
    if (r)
        return -r;
    r = strcmp(a->first_name, b->first_name);
    if (r)
        return -r;
    if (a->id < b->id)
        return a->id > b->id ? -1 : 1;
    return 0;
}

typedef struct EmployeeArray {
    int size;
    int cap;
    Employee *ptr;
} EmployeeArray;

int employee_array_init(EmployeeArray *arr) {
    arr->size = 0;
    arr->cap = 128;
    arr->ptr = malloc(128 * sizeof(Employee));
    if (arr->ptr == NULL) {
        return S_MALLOC_ERROR;
    }
    return S_OK;
}

int employee_array_push(EmployeeArray *arr, Employee e) {
    if (arr->size >= arr->cap) {
        int new_cap = arr->cap * 2;
        Employee *tmp = realloc(arr->ptr, new_cap);
        if (tmp == NULL)
            return S_MALLOC_ERROR;
        arr->cap = new_cap;
        arr->ptr = tmp;
    }
    arr->ptr[arr->size] = e;
    arr->size++;
    return S_OK;
}

int copy_string_field(char **field, char **start, char **end) {
    *end = strchr(*start, ';');
    if (!(*end)) {
        fprintf(stderr, "ERROR: not enough fields in line\n");
        return S_PARSE_NOT_ENOUGH_FIELDS_ERROR;
    }
    int len = *end - *start;
    *field = malloc(len + 1);
    if (*field == NULL)
        return S_MALLOC_ERROR;
    memcpy(*field, *start, len);
    (*field)[len] = 0;
    return S_OK;
}

int employee_array_read_file(EmployeeArray *arr, FILE *fd) {
    if (arr->ptr == NULL || arr->cap == 0) {
        int r = employee_array_init(arr);
        if (r)
            return r;
    }
    size_t line_len = 0;
    char *line = NULL;
    while (true) {
        Employee a = {0};
        int n = getline(&line, &line_len, fd);
        if (n <= 1)
            break;
        char *start = line;
        char *end;
        a.id = strtoul(start, &end, 10);
        if (*end != ';') {
            if (*end != 0)
                *(end + 1) = 0;
            fprintf(stderr, "ERROR: failed to parse \"%s\"\n", start);
            free(line);
            return S_PARSE_NUMBER_ERROR;
        }
        start = end + 1;
        int r = copy_string_field(&a.first_name, &start, &end);
        if (r)
            return r;

        start = end + 1;
        r = copy_string_field(&a.last_name, &start, &end);
        if (r)
            return r;

        start = end + 1;
        a.pay = strtof(start, &end);
        if (*end != ';' && *end != '\n') {
            if (*end != 0)
                *(end + 1) = 0;
            fprintf(stderr, "ERROR: failed to parse \"%s\"\n", start);
            free(line);
            return S_PARSE_NUMBER_ERROR;
        }
        if (*end != '\n') {
            fprintf(stderr, "ERROR: too many fields in line \n");
            free(line);
            return S_PARSE_TOO_MANY_FIELDS_ERROR;
        }
        r = employee_array_push(arr, a);
        if (r)
            return r;
    }

    if (line)
        free(line);

    return 0;
}

int employee_array_sort(EmployeeArray *arr, cmp comp) {
    qsort(arr->ptr, arr->size, sizeof(Employee), comp);
    return S_OK;
}

int employee_array_free(EmployeeArray *arr) {
    for (int i = 0; i < arr->size; i++) {
        free(arr->ptr[i].first_name);
        free(arr->ptr[i].last_name);
    }
    free(arr->ptr);
    arr->cap = 0;
    arr->size = 0;
    return S_OK;
}

int employee_array_write_file(EmployeeArray *arr, FILE *fd) {
    for (int i = 0; i < arr->size; i++) {
        Employee a = arr->ptr[i];
        int r = fprintf(fd, "%lu;%s;%s;%f\n", a.id, a.first_name, a.last_name,
                        a.pay);
        if (r < 0)
            return S_WRITE_ERROR;
    }
    return S_OK;
}

int main(int argc, const char *argw[]) {
    if (argc < 4) {
        fprintf(stderr, "ERROR: not enough args: expected 3, got: %d\n",
                argc - 1);
        return 1;
    }
    const char *flags[] = {"-d", "/d", "-a", "/a"};
    const cmp cmps[] = {cmp_des, cmp_asc};
    cmp comp = NULL;
    for (int i = 0; i < 4; i++) {
        if (strcmp(argw[2], flags[i]) == 0) {
            comp = cmps[i / 2];
            break;
        }
    }
    if (!comp) {
        fprintf(stderr, "ERROR: unknown flag: \"%s\"\n", argw[2]);
        return 1;
    }

    FILE *in = fopen(argw[1], "r");
    if (!in) {
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", argw[1]);
        return 1;
    }

    EmployeeArray arr = {0};
    int res = employee_array_read_file(&arr, in);
    if (res) {
        employee_array_free(&arr);
        fclose(in);
        return res;
    }

    FILE *out = fopen(argw[3], "w");
    if (!out) {
        employee_array_free(&arr);
        fclose(in);
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", argw[3]);
        return 1;
    }

    employee_array_sort(&arr, comp);

    res = employee_array_write_file(&arr, out);

    employee_array_free(&arr);
    fclose(in);
    fclose(out);
    return res;
}
