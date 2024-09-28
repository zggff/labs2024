#include <math.h>
#include <stdio.h>

#define EPSILON 0.0000000001

typedef int (*func)(double x, double *res);

int round_to_digits(int digits, double *val) {
    if (digits <= 0)
        return 1;
    long long mul = pow(10, digits);
    long long res_ll = (long long)(*val * mul + 0.5);
    *val = (double)res_ll / mul;
    return 0;
}

int calculate_breakpoint(func f, double x, double *out) {
    if (f(x, out) == 0)
        return 0;

    double y0 = 0;
    double y1 = 0;
    int left = f(x - EPSILON, &y0);
    int right = f(x + EPSILON, &y1);
    if (left == 0 && right == 0)
        *out = (y0 + y1) / 2;
    else if (left == 0)
        *out = y0;
    else if (right == 0)
        *out = y1;
    else
        return 1;
    return 0;
}

int integrate_with_step(func f, double start, double end, long steps,
                        double *out) {
    double step = (end - start) / steps;
    double x0 = start;
    double x1 = x0 + step;
    double y0 = 0;
    double y1 = 0;
    int res = 0;

    if ((res = calculate_breakpoint(f, x0, &y1))) {
        return res;
    };
    *out = 0;

    for (long i = 0; i < steps; i++) {
        if ((res = calculate_breakpoint(f, x1, &y1))) {
            return res;
        };
        *out += (y0 + y1) * step / 2;
        x0 = x1;
        x1 += step;
        y0 = y1;
    }

    return 0;
}

int integrate(func f, double start, double end, double epsilon, double *out) {
    long steps = 2;
    double prev = 0;
    int digits = ceil(fabs(log10(epsilon)));

    *out = 0;
    do {
        prev = *out;
        if (integrate_with_step(f, start, end, steps, out))
            return 1;
        steps *= 2;
    } while (fabs(*out - prev) > epsilon);
    return round_to_digits(digits, out);
}

int calc_a(double x, double *y) {
    if (x == -1 || x == 0) {
        return 1;
    }
    *y = log(1 + x) / x;
    return 0;
}

int calc_b(double x, double *y) {
    *y = exp(-(x * x / 2));
    return 0;
}

int calc_c(double x, double *y) {
    if (x >= 1) {
        return 1;
    }
    *y = log(1 / (1 - x));
    return 0;
}

int calc_d(double x, double *y) {
    *y = pow(x, x);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "ERROR: not enough args\n");
        return 1;
    }
    double epsilon;
    if (sscanf(argv[1], "%lf", &epsilon) != 1) {
        fprintf(stderr, "ERROR: epsilon must be numbers\n");
        return 1;
    }
    if (epsilon <= 0 || epsilon >= 1) {
        fprintf(stderr, "ERROR: epsilon must be lesser than 1\n");
        return 1;
    }

    int digits = ceil(fabs(log10(epsilon)));

    func funcs[] = {calc_a, calc_b, calc_c, calc_d};
    for (int i = 0; i < (int)(sizeof(funcs) / sizeof(func)); i++) {
        char c = 'a' + i;
        double res = 0;
        int s = integrate(funcs[i], 0, 1, epsilon, &res);
        if (s) {
            fprintf(stderr, "%c)\tERROR: %d\n", c, s);
        } else {
            printf("%c)\t%.*f\n", c, digits, res);
        }
    }

    return 0;
}
