#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int swap_min_max(int *min, int *max, int *arr, size_t size) {
    size_t min_pos = 0;
    size_t max_pos = 0;
    for (size_t i = 0; i < size; i++) {
        if (arr[i] > arr[max_pos])
            max_pos = i;
        if (arr[i] < arr[min_pos])
            min_pos = i;
    }
    *max = arr[max_pos];
    *min = arr[min_pos];
    int tmp = arr[min_pos];
    arr[min_pos] = arr[max_pos];
    arr[max_pos] = arr[tmp];
    return 0;
}

int func_one(int a, int b) {
    int arr[128];
    int size = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % (b - a + 1) + a;
    }
    int max, min;
    swap_min_max(&min, &max, arr, size);
    printf("min=%d, max=%d\n", min, max);
    return 0;
}

int func_two(void) {
    int size_a = rand() % (10000 - 10 + 1) + 10;
    int size_b = rand() % (10000 - 10 + 1) + 10;
    int *arr_a = malloc(size_a * sizeof(int));
    int *arr_b = malloc(size_b * sizeof(int));
    int *arr_c = malloc(size_a * sizeof(int));
    if (arr_a == NULL || arr_b == NULL || arr_c == NULL) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
    }
    for (int i = 0; i < size_a; i++) {
        arr_a[i] = rand() % (1000 - (-1000) + 1) + (-1000);
    }
    for (int i = 0; i < size_b; i++) {
        arr_b[i] = rand() % (1000 - (-1000) + 1) + (-1000);
    }
    for (int i = 0; i < size_a; i++) {
        int closest = 0;
        for (int j = 0; j < size_b; j++) {
            if (abs(arr_a[i] - arr_b[j]) < abs(arr_a[i] - closest))
                closest = arr_b[j];
        }
        arr_c[i] = arr_a[i] + closest;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "ERROR: no command line arguments provided\n");
        return 1;
    }
    if (strcmp(argv[1], "-1") == 0) {
        if (argc <= 3) {
            fprintf(stderr, "ERROR: a and b not provided\n");
            return 1;
        }
        int a, b;
        if (sscanf(argv[2], "%d", &a) != 1 || sscanf(argv[3], "%d", &b) != 1) {
            fprintf(stderr, "ERROR: arguments must be numbers\n");
            return 1;
        }
        return func_one(a, b);
    } else if (strcmp(argv[1], "-2") == 0) {
        return func_two();
    } else {
        fprintf(stderr, "ERROR: unknown argument: \"%s\"\n", argv[1]);
    }
}
