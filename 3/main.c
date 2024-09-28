#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef int (*handle)(int argc, const char *argv[]);

int validate_float(const char *s, bool *res) {
    int dot_count = 0;
    *res = 1;
    if (*s == '-')
        s++;
    for (; *s != 0; s++) {
        if (*s == '.') {
            dot_count++;
            if (dot_count > 1) {
                *res = 0;
                return 0;
            }
            continue;
        }
        if (!('0' <= *s && *s <= '9')) {
            *res = 0;
            return 0;
        }
    }
    return 0;
}

int validate_int(const char *s, bool *res) {
    *res = 1;
    if (*s == '-')
        s++;
    for (; *s != 0; s++) {
        if (!('0' <= *s && *s <= '9')) {
            *res = 0;
            return 0;
        }
    }
    return 0;
}

int solve_quadratic(float e, float a, float b, float c, int *out) {
    (void)out;

    // ax^2 + bx + c = 0
    printf("%f*x^2 + %f*x + %f = 0\n", a, b, c);
    float x1 = 0;
    float x2 = 0;
    if (fabs(a - 0) < e) { // bx + c = 0 => bx = c => x = c/b
        x1 = c / b;
        printf("\tx=%f\n", x1);
        return 0;
    }

    float d = b * b - 4 * a * c;
    if (d < 0) {
        printf("\tthe equation has no solutions\n");
        return 0;
    }

    if (fabs(d - 0) < e) { // x = -b/(2 * a)
        x1 = -b / (2 * a);
        printf("\tx=%f\n", x1);
        return 0;
    }
    d = sqrt(d);
    x1 = (-b - d) / (2 * a);
    x2 = (-b + d) / (2 * a);
    printf("\tx1=%f,\tx2=%f\n", x1, x2);
    return 0;
}

int handle_combinations(float e, float vals[3],
                        int (*func)(float, float, float, float, int *),
                        int *out) {
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            for (int c = 0; c < 3; c++) {
                if (a == b || b == c || c == a)
                    continue;
                int res = func(e, vals[a], vals[b], vals[c], out);
                if (res != 0)
                    return res;
            }
        }
    }
    return 0;
}

int handle_q(int argc, const char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "ERROR: not enough arguments provided\n");
        return 1;
    }
    float e;
    float coef[3];
    bool valid;
    validate_float(argv[0], &valid);
    if (!valid) {
        fprintf(stderr, "ERROR: epsilon must be a number\n");
        return 1;
    }

    if (sscanf(argv[0], "%f", &e) != 1) {
        fprintf(stderr, "ERROR: could not parse arguments as numbers\n");
        return 1;
    }
    if (e <= 0) {
        fprintf(stderr, "ERROR: epsilon must be positive\n");
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        validate_float(argv[1 + i], &valid);
        if (!valid) {
            fprintf(stderr, "ERROR: nums must be a number\n");
            return 1;
        }
        if (sscanf(argv[1 + i], "%f", &coef[i]) != 1) {
            fprintf(stderr, "ERROR: could not parse arguments as numbers\n");
            return 1;
        }
    }
    return handle_combinations(e, coef, solve_quadratic, NULL);
}

int handle_m(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: not enough arguments provided\n");
        return 1;
    }
    int a, b;
    bool valid;
    validate_int(argv[0], &valid);
    if (!valid) {
        fprintf(stderr, "ERROR: first number must be a number\n");
        return 1;
    }
    validate_int(argv[1], &valid);
    if (!valid) {
        fprintf(stderr, "ERROR: second number must be a number\n");
        return 1;
    }

    if (sscanf(argv[0], "%d", &a) != 1 || sscanf(argv[1], "%d", &b) != 1) {
        fprintf(stderr, "ERROR: could not parse arguments as numbers\n");
        return 1;
    }
    if (b == 0 || a == 0) {
        fprintf(stderr, "ERROR: numbers cannot be zero\n");
        return 1;
    }
    if (a % b == 0) {
        printf("%d is divisible by %d\n", a, b);
    } else {
        printf("%d is not divisible by %d\n", a, b);
    }
    return 0;
}

int check_triangle(float e, float a, float b, float c, int *out) {
    if (fabs(a * a + b * b - c * c) < e) {
        *out = 1;
    }
    return 0;
}

int handle_t(int argc, const char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "ERROR: not enough arguments provided\n");
        return 1;
    }
    float e;
    float sides[3];
    bool valid;
    validate_float(argv[0], &valid);
    if (!valid) {
        fprintf(stderr, "ERROR: epsilon must be a number\n");
        return 1;
    }

    if (sscanf(argv[0], "%f", &e) != 1) {
        fprintf(stderr, "ERROR: could not parse arguments as numbers\n");
        return 1;
    }
    if (e <= 0) {
        fprintf(stderr, "ERROR: epsilon must be positive\n");
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        validate_float(argv[1 + i], &valid);
        if (!valid) {
            fprintf(stderr, "ERROR: sides must be numbers\n");
            return 1;
        }
        if (sscanf(argv[1 + i], "%f", &sides[i]) != 1) {
            fprintf(stderr, "ERROR: could not parse arguments as numbers\n");
            return 1;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (sides[i] <= 0) {
            fprintf(stderr, "ERROR: numbers must be positive\n");
            return 1;
        }
    }
    int sol = 0;
    int r = handle_combinations(e, sides, check_triangle, &sol);
    if (r != 0)
        return r;
    if (sol) {
        printf("%f, %f, %f can be sides of a right-angled triangle\n", sides[0],
               sides[1], sides[2]);
    } else {
        printf("%f, %f, %f can not be sides of a right-angled triangle\n",
               sides[0], sides[1], sides[2]);
    }

    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "ERROR: no args provided\n");
        return 1;
    }
    const char *flags[] = {"-q", "/q", "-m", "/m", "-t", "/t"};
    handle handles[] = {handle_q, handle_m, handle_t};
    for (int i = 0; i < (int)(sizeof(flags) / sizeof(flags[0])); i++) {
        if (strcmp(flags[i], argv[1]) == 0) {
            return handles[i / 2](argc - 2, argv + 2);
        }
    }

    fprintf(stderr, "ERROR: flag not recognised \"%s\"\n", argv[1]);
    return 1;
}
