#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef int (*handle)(int x);

int handle_h(int x) {
    bool found = false;
    for (int i = 1; i <= 100; i++) {
        if (i % x == 0) {
            printf("%d\n", i);
            found = true;
        }
    }
    if (!found) {
        printf("no numbers divisible by %d found\n", x);
    }
    return 0;
}

int handle_p(int x) {
    for (int i = 2; i < x / 2; i++) {
        if (x % i == 0) {
            printf("%d is not a prime\n", x);
            return 0;
        }
    }
    printf("%d is a prime\n", x);
    return 0;
}

int handle_s(int x) {
    bool started = false;
    for (int i = sizeof(x) * 8 - 4; i >= 0; i -= 4) {
        int digit =
            (x & (1 << i | 1 << (i + 1) | 1 << (i + 2) | 1 << (i + 3))) >> i;
        if (digit != 0)
            started = true;
        if (started)
            printf("%d ", digit);
    }

    printf("\n");
    return 0;
}

int handle_e(int x) {
    if (x > 10) {
        fprintf(stderr, "ERROR: x must not be greater than 10\n");
        return 1;
    }
    for (int base = 1; base <= 10; base++) {
        printf("%d:\t", base);
        for (int exp = 1; exp <= x; exp++) {
            long res = pow(base, exp);
            printf("%ld, ", res);
        }
        printf("\n");
    }
    return 0;
}

int handle_a(int x) {
    long sum = (1 + x) * (x) / 2;
    printf("%ld\n", sum);
    return 0;
}

int handle_f(int x) {
    long long fact = 1;
    long long prev = 1;
    for (int i = 1; i <= x; i++) {
        prev = fact;
        fact *= i;
        if (fact / i != prev) {
            fprintf(stderr, "ERROR: number overflow\n");
            return 1;
        }
    }
    printf("%lld\n", fact);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: operation and number must be both provided\n");
        return 1;
    }
    if (argc > 3) {
        fprintf(stderr,
                "ERROR: program accepts only one number and one operation\n");
        return 1;
    }

    int x;
    if (sscanf(argv[1], "%d", &x) != 1) {
        fprintf(stderr, "ERROR: couldn't parse \"%s\" as number\n", argv[1]);
        return 1;
    }
    if (x <= 0) {
        fprintf(stderr, "ERROR: x must be a natural number\n");
        return 1;
    }
    const char *flags[] = {"-h", "/h", "-p", "/p", "-s", "/s",
                           "-e", "/e", "-a", "/a", "-f", "/f"};
    const handle handles[] = {handle_h, handle_p, handle_s,
                              handle_e, handle_a, handle_f};

    for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
        if (strcmp(argv[2], flags[i]) == 0) {
            return handles[i / 2](x);
        }
    }

    fprintf(stderr, "ERROR: flag not recognised \"%s\"\n", argv[2]);
    return 1;
}
