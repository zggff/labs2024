#include <math.h>
#include <stdbool.h>
#include <limits.h>

#include "calc_last.h"
#include "ops.h"

int limit_sum(double epsilon, double m, double *res) {
    *res = 0;
    double k = 2;
    double fact_m;
    double fact_k;
    double fact_mk;
    double prev;
    while (k <= m) {
        prev = *res;
        if (factorial_d(m, &fact_m) || factorial_d(k, &fact_k) ||
            factorial_d(m - k, &fact_mk))
            return MATH_OVERFLOW;
        double diff = fact_m / fact_k * log(fact_k) * pow(-1, k) / k / fact_mk;
        *res += diff;
        k++;
        if (fabs(*res - prev) < epsilon)
            break;
    }
    return MATH_OKAY;
}

int calculate_limit_y(double epsilon, double *res) {
    double sum = 0;
    *res = 0;
    double m = 4;
    double prev = 0;
    while (m < 60) {
        prev = *res;
        int r = limit_sum(epsilon, m, &sum);
        if (sum < *res)
            break;
        *res = sum;
        (void)r;
        m++;
        if (fabs(*res - prev) < epsilon)
            break;
    }
    return MATH_OKAY;
}

int series_sum(double epsilon, double *res) {
    double k = 1;
    double k_max = pow(2, 16);
    *res = 0;
    double diff = 0;
    double n = 4;
    while (k < k_max) {
        diff = 0;
        for (int i = 0; i < n; i++) {
            k++;
            double s = sqrt(k);
            if (s == (int)s) {
                continue;
            }
            diff += (1 / pow(floor(s), 2)) - (1 / k);
        }
        if (n < pow(2, 16))
            n *= 2;
        *res += diff;
        if (diff < epsilon)
            break;
    };
    return MATH_OKAY;
}

int calculate_series_y(double epsilon, double *res) {
    double sum;
    series_sum(epsilon, &sum);
    *res = (-M_PI * M_PI / 6) + sum;
    int digits = ceil(fabs(log10(epsilon)));
    return round_to_digits(digits, res);
}

int check_prime(ll n, bool *res) {
    *res = true;
    for (ll i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            *res = false;
            return MATH_OKAY;
        }
    }
    return MATH_OKAY;
}

int equation_mul(double epsilon, double upper, double *p, bool *reach,
                 double *res) {
    bool is_prime = false;
    double prev = 1;
    *reach = false;
    while (true) {
        if ((*p) >= upper)
            break;
        (*p)++;
        check_prime((int)(*p), &is_prime);
        if (!is_prime) {
            continue;
        }
        prev = *res;
        *res *= (*p - 1) / (*p);

        if (fabs(*res - prev) < epsilon) {
            *reach = true;
            break;
        }
    };
    return MATH_OKAY;
}

int equation_lim(double epsilon, double *res) {
    double prev;
    double n = 2;
    double p = 1;
    double mul = 1;
    bool mul_done = false;
    do {
        prev = *res;
        equation_mul(epsilon, n, &p, &mul_done, &mul);

        *res = log(n) * mul;
        if (isinf(*res)) {
            *res = prev;
            break;
        }
        n *= 2;
    } while (fabs(*res - prev) >= epsilon && !mul_done);
    return MATH_OKAY;
}

int calc_e(double x, double *res) {
    *res = exp(-x);
    return 0;
}

int calculate_equation_y(double epsilon, double *res) {
    *res = 0;
    double expected = 0;
    equation_lim(epsilon, &expected);
    calculate_equation_binsearch(epsilon, calc_e, false, expected, 0, 1, res);
    int digits = ceil(fabs(log10(epsilon)));
    return round_to_digits(digits, res);
}
