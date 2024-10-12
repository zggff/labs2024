#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum Status {
    S_OK = 0,
    S_INVALID_INPUT = 1,
    S_MALLOC_ERROR = 2,
} Status;

typedef struct Vector {
    int n;
    float *coord;
} Vector;

#define vec_init(vec, coords)                                                  \
    {                                                                          \
        vec.coord = coords;                                                    \
        vec.n = sizeof(coords) / sizeof(coords[0]);                            \
    }

typedef int (*norm)(float *res, const Vector *v);

int vec_print(const Vector *v) {
    for (int i = 0; i < v->n; i++) {
        printf("%f", v->coord[i]);
        if (i < v->n - 1)
            printf(" ");
    }
    printf("\n");
    return S_OK;
}

typedef struct Array {
    int elem_size;
    int size;
    int capacity;
    void *inner;
} Array;

int array_init(Array *a, int capacity, int elem_size) {
    a->capacity = capacity;
    a->elem_size = elem_size;
    a->inner = malloc(a->capacity * a->elem_size);
    a->size = 0;
    if (!a->inner)
        return S_MALLOC_ERROR;
    return S_OK;
}

int array_free(Array *a) {
    free(a->inner);
    return S_OK;
}

int array_push(Array *a, const void *val) {
    if (a->size >= a->capacity) {
        int new_capacity = a->capacity * 2;
        void *tmp = realloc(a->inner, new_capacity * a->elem_size);
        if (!tmp)
            return S_MALLOC_ERROR;
        a->inner = tmp;
        a->capacity = new_capacity;
    }
    memcpy(a->inner + a->size * a->elem_size, val, a->elem_size);
    a->size++;
    return S_OK;
}

int array_get(void *val, Array *a, int i) {
    if (i >= a->size)
        return S_INVALID_INPUT;
    memcpy(val, a->inner + i * a->elem_size, a->elem_size);
    return S_OK;
}

int handle(int n, ...) {
    if (n <= 0)
        return S_INVALID_INPUT;

    Array vecs;
    if (array_init(&vecs, 16, sizeof(Vector)))
        return S_MALLOC_ERROR;
    Array norms;
    if (array_init(&norms, 16, sizeof(norm))) {
        array_free(&vecs);
        return S_MALLOC_ERROR;
    }

    va_list valist;
    va_start(valist, n);

    while (true) {
        Vector *v = va_arg(valist, Vector *);
        if (!v)
            break;
        if (v->n != n) {
            array_free(&vecs);
            va_end(valist);
            return S_INVALID_INPUT;
        }
        if (array_push(&vecs, v)) {
            array_free(&vecs);
            va_end(valist);
            return S_MALLOC_ERROR;
        }
    }

    while (true) {
        norm n = va_arg(valist, norm);
        if (!n)
            break;
        // n(NULL);
        if (array_push(&norms, &n)) {
            array_free(&norms);
            array_free(&vecs);
            va_end(valist);
            return S_MALLOC_ERROR;
        }
    }

    for (int i = 0; i < norms.size; i++) {
        norm f;
        array_get(&f, &norms, i);
        float max = 0;
        float res;
        for (int j = 0; j < vecs.size; j++) {
            Vector v;
            array_get(&v, &vecs, j);
            f(&res, &v);
            if (res > max)
                max = res;
        }
        printf("%d %f\n", i, max);
        for (int j = 0; j < vecs.size; j++) {
            Vector v;
            array_get(&v, &vecs, j);
            f(&res, &v);
            if (res == max) {
                printf("\t");
                vec_print(&v);
            }
        }
    }

    va_end(valist);
    array_free(&vecs);
    array_free(&norms);
    return S_OK;
}

int calc_norm_max(float *res, const Vector *v) {
    float max = 0;
    for (int i = 0; i < v->n; i++) {
        if (fabsf(v->coord[i]) > max) {
            max = fabsf(v->coord[i]);
        }
    }
    *res = max;
    return 0;
}

int pow_custom(double *res, double x, int n) {
    bool rev = n < 0;
    if (rev)
        n *= -1;
    *res = 1;
    for (;;) {
        if (n & 1)
            (*res) *= x;
        n >>= 1;
        if (!n)
            break;
        x *= x;
    }
    if (rev)
        *res = 1 / *res;
    return 0;
}

int calc_norm_p(float *res, const Vector *v, int p) {
    float sum = 0;
    for (int i = 0; i < v->n; i++) {
        float ab = fabsf(v->coord[i]);
        double abp;
        pow_custom(&abp, ab, p);
        sum+= abp;
        // sum += pow(ab, p);
    }
    *res = pow(sum, 1.0 / p);
    return 0;
}

int calc_norm_mat(float *res, const Vector *v, const float *mat) {
    float sum = 0;
    for (int i = 0; i < v->n; i++) {
        float sum2 = 0;
        for (int j = 0; j < v->n; j++) {
            int pos = i * v->n + j;
            sum2 += mat[pos] * v->coord[j];
        }
        sum += sum2 * v->coord[i];
    }
    *res = sqrtf(sum);
    return 0;
}

int calc_norm_1(float *res, const Vector *v) {
    return calc_norm_max(res, v);
}

int calc_norm_2(float *res, const Vector *v) {
    return calc_norm_p(res, v, 2);
}

int calc_norm_3(float *res, const Vector *v) {
    float a[] = {2, 0, 0, 2};
    return calc_norm_mat(res, v, a);
}

int main(void) {
    Vector a, b, c, d;
    float a_coord[] = {0.5, -5};
    vec_init(a, a_coord);
    float b_coord[] = {5.0, 0.0};
    vec_init(b, b_coord);
    float c_coord[] = {4, 4};
    vec_init(c, c_coord);
    float d_coord[] = {-4, 4};
    vec_init(d, d_coord);

    handle(2, &a, &b, &c, &d, NULL, calc_norm_1, calc_norm_2, calc_norm_3,
           NULL);
}
