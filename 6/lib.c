#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "lib.h"

int parse_field_str(char **res, char const **start, mask m) {
    const char *end = *start;
    while (*end && !m(*end)) {
        end++;
    }
    int n = end - *start;
    *res = malloc(n + 1);
    if (!res)
        return S_MALLOC_ERROR;
    memcpy(*res, *start, n);
    (*res)[n] = 0;
    *start = end + 1;
    return S_OK;
}

int parse_field_uint(unsigned long *res, char const **start, mask m) {
    char *ptr;
    *res = strtoul(*start, &ptr, 10);
    bool valid = *ptr == 0;
    valid = valid | m(*ptr);
    if (!valid) {
        ptr++;
        char tmp = *ptr;
        *ptr = 0;
        fprintf(stderr, "ERROR: failed to parse [%s] as unsigned\n", *start);
        *ptr = tmp;
        return S_PARSE_ERROR;
    }

    *start = ptr + 1;
    return S_OK;
}

int parse_field_float(float *res, char const **start, mask m) {
    char *ptr;
    *res = strtof(*start, &ptr);
    bool valid = *ptr == 0;
    valid = valid | m(*ptr);
    if (!valid) {
        ptr++;
        char tmp = *ptr;
        *ptr = 0;
        fprintf(stderr, "ERROR: failed to parse [%s] as float\n", *start);
        *ptr = tmp;
        return S_PARSE_ERROR;
    }

    *start = ptr + 1;
    return S_OK;
}

int parse_field_time(tm *res, const char **start, mask m) {
    char *str;
    parse_field_str(&str, start, m);
    char *last = strptime(str, "%d.%m.%Y %H:%M:%S", res);
    if (last == NULL || *last != 0) {
        fprintf(stderr, "ERROR: failed to parse [%s] as date\n", str);
        free(str);
        return S_PARSE_ERROR;
    }
    res->tm_isdst = -1;
    free(str);
    return S_OK;
}
int stop_print(const Stop *s) {
    char in[26];
    strftime(in, 26, "%d.%m.%Y %H:%M:%S", &s->in);
    char out[26];
    strftime(out, 26, "%d.%m.%Y %H:%M:%S", &s->out);
    char type[20];
    switch (s->type) {
    case StopStart:
        strcpy(type, "start");
        break;
    case StopMiddle:
        strcpy(type, "intermediate");
        break;
    case StopEnd:
        strcpy(type, "end");
        break;
    };
    printf("%s (%f, %f) [%s]-[%s]", type, s->lat, s->lon, in, out);
    return S_OK;
}

int route_init(Route *r) {
    r->cap = 16;
    r->stops = malloc(r->cap * sizeof(Stop));
    return r->stops ? S_OK : S_MALLOC_ERROR;
}
int route_free(Route *r) {
    free(r->stops);
    free(r->id);
    return S_OK;
}
int route_print(const Route *r) {
    printf("%s:\t%zu\n", r->id, r->size);
    for (size_t i = 0; i < r->size; i++) {
        printf("\t[%02zu]: ", i);
        stop_print(&r->stops[i]);
        printf("\n");
    }
    return S_OK;
}

int route_push(Route *r, Stop s) {
    if (r->size >= r->cap) {
        size_t new_cap = r->cap * 2;
        Stop *t = realloc(r->stops, new_cap * sizeof(Stop));
        if (!t)
            return S_MALLOC_ERROR;
        r->stops = t;
        r->cap = new_cap;
    }
    r->stops[r->size] = s;
    r->size++;
    return S_OK;
}

int routes_init(Routes *r) {
    r->cap = 16;
    r->ptr = malloc(r->cap * sizeof(Route));
    return r->ptr ? S_OK : S_MALLOC_ERROR;
}
int routes_free(Routes *r) {
    for (size_t i = 0; i < r->size; i++) {
        route_free(&r->ptr[i]);
    }
    free(r->ptr);
    return S_OK;
}
int routes_print(const Routes *r) {
    for (size_t i = 0; i < r->size; i++) {
        printf("[%zu]: ", i);
        route_print(&r->ptr[i]);
    }
    return S_OK;
}

int routes_push(Routes *r, Route s) {
    if (r->size >= r->cap) {
        size_t new_cap = r->cap * 2;
        Route *t = realloc(r->ptr, new_cap * sizeof(Route));
        if (!t)
            return S_MALLOC_ERROR;
        r->ptr = t;
        r->cap = new_cap;
    }
    r->ptr[r->size] = s;
    r->size++;
    return S_OK;
}

int routes_push_with_id(Routes *r, const char *id) {
    Route ro = {0};
    check(route_init(&ro), {});
    int n = strlen(id);
    ro.id = malloc(n + 1);
    if (ro.id == NULL)
        return S_MALLOC_ERROR;
    strcpy(ro.id, id);
    return routes_push(r, ro);
}

int is_sep(int c) {
    return c == ';';
}

int routes_parse_file(Routes *r, FILE *f) {
    size_t line_len = 0;
    char *line = NULL;
    int n = getline(&line, &line_len, f);
    if (n <= 0)
        return S_PARSE_ERROR;

    const char *s = line;
    float lat, lon;
    check(parse_field_float(&lat, &s, isspace), free(line));
    check(parse_field_float(&lon, &s, isspace), free(line));

    while (true) {
        int n = getline(&line, &line_len, f);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;
        const char *s = line;

        char *id;
        tm in;
        tm out;
        unsigned long type;
        check(parse_field_str(&id, &s, is_sep), free(line));
        check(parse_field_time(&in, &s, is_sep), free(line); free(id));
        check(parse_field_time(&out, &s, is_sep), free(line); free(id));
        check(parse_field_uint(&type, &s, is_sep), free(line); free(id));
        if (*id == 0) {
            fprintf(stderr, "ERROR: id must not be empty\n");
            free(line);
            free(id);
            return S_PARSE_ERROR;
        }
        if (type > 2) {
            fprintf(stderr, "ERROR: stop type can only be in [0;2] range");
            free(line);
            free(id);
            return S_PARSE_ERROR;
        }

        Route *ro = NULL;
        for (size_t i = 0; i < r->size; i++) {
            if (strcmp(r->ptr[i].id, id) == 0) {
                ro = &r->ptr[i];
                break;
            }
        }
        if (ro == NULL) {
            check(routes_push_with_id(r, id), free(line); free(id));
            ro = &r->ptr[r->size - 1];
        }
        free(id);
        Stop so = {.lat = lat, .lon = lon, .in = in, .out = out, .type = type};
        check(route_push(ro, so), free(line));
    }
    if (line)
        free(line);
    return S_OK;
}

int stop_cmp(const void *a0, const void *b0) {
    const Stop *a = a0;
    const Stop *b = b0;
    tm ta = a->in;
    tm tb = b->in;
    double diff = difftime(mktime(&ta), mktime(&tb));
    return (diff > 0) - (diff < 0);
}

int route_filter(Route *r) {
    qsort(r->stops, r->size, sizeof(Stop), stop_cmp);
    return S_OK;
}

int routes_filter(Routes *r) {
    for (size_t i = 0; i < r->size; i++)
        route_filter(&r->ptr[i]);
    return S_OK;
}

int routes_find_ext_routes_cnt(Route **res, const Routes *r, bool max) {
    *res = NULL;
    int ext_cnt = max ? 0 : INT_MAX;
    for (size_t i = 0; i < r->size; i++) {
        Route *ro = &r->ptr[i];
        int cnt = 0;
        for (size_t j = 0; j < ro->size; j++) {
            if (ro->stops[j].type != 1)
                cnt++;
        }
        cnt = (cnt + 1) / 2;
        if ((cnt > ext_cnt) ^ !max) {
            *res = ro;
            ext_cnt = cnt;
        }
    }
    return S_OK;
}
int routes_find_ext_path_len(Route **res, const Routes *r, bool max) {
    *res = NULL;
    double ext_dur = max ? 0 : INFINITY;
    for (size_t i = 0; i < r->size; i++) {
        Route *a = &r->ptr[i];
        double dur = 0;
        for (size_t j = 0; j < a->size - 1; j++) {
            tm out = a->stops[j].out;
            tm in = a->stops[j + 1].in;
            dur += difftime(mktime(&in), mktime(&out));
        }
        if ((dur > ext_dur) ^ !max) {
            *res = a;
            ext_dur = dur;
        }
    }
    return S_OK;
}
int routes_find_ext_route_len(Route **res, const Routes *r, bool max) {
    *res = NULL;
    double ext_dur = max ? 0 : INFINITY;
    for (size_t i = 0; i < r->size; i++) {
        Route *a = &r->ptr[i];
        double dur = 0;
        for (size_t j = 0; j < a->size - 1; j++) {
            if (a->stops[j].type == StopStart)
                dur = 0;
            if (a->stops[j].type == StopEnd)
                break;
            tm out = a->stops[j].out;
            tm in = a->stops[j + 1].in;
            dur += difftime(mktime(&in), mktime(&out));
        }
        if ((dur > ext_dur) ^ !max) {
            *res = a;
            ext_dur = dur;
        }
    }
    return S_OK;
}

int routes_find_ext_stop_dur(Route **res, const Routes *r, bool max) {
    *res = NULL;
    double ext_dur = max ? 0 : INFINITY;
    for (size_t i = 0; i < r->size; i++) {
        Route *ro = &r->ptr[i];
        double dur = max ? 0 : INFINITY;
        for (size_t j = 0; j < ro->size; j++) {
            tm a = ro->stops[j].in;
            tm b = ro->stops[j].out;
            double d = difftime(mktime(&b), mktime(&a));
            if ((d > dur) ^ !max) {
                dur = d;
            }
        }
        if ((dur > ext_dur) ^ !max) {
            *res = ro;
            ext_dur = dur;
        }
    }
    return S_OK;
}
int routes_find_ext_stop_total_dur(Route **res, const Routes *r, bool max) {
    *res = NULL;
    double ext_dur = max ? 0 : INFINITY;
    for (size_t i = 0; i < r->size; i++) {
        Route *ro = &r->ptr[i];
        double dur = 0;
        for (size_t j = 0; j < ro->size; j++) {
            tm a = ro->stops[j].in;
            tm b = ro->stops[j].out;
            dur += difftime(mktime(&b), mktime(&a));
        }
        if ((dur > ext_dur) ^ !max) {
            *res = ro;
            ext_dur = dur;
        }
    }
    return S_OK;
}
