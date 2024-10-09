#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define check(MACRO, AFTER)                                                    \
    {                                                                          \
        int _r = MACRO;                                                        \
        if (_r) {                                                               \
            AFTER;                                                             \
            return _r;                                                         \
        }                                                                      \
    }

typedef enum Status {
    S_OK = 0,
    S_MALLOC_ERROR = 1,
    S_PARSE_ERROR = 2,
} Status;

typedef enum StopType { StopStart, StopMiddle, StopEnd } StopType;

typedef struct tm tm;

typedef struct Stop {
    float lat;
    float lon;
    StopType type;
    tm in;
    tm out;
} Stop;

int stop_print(const Stop *s);

typedef struct Route {
    char *id;
    size_t cap;
    size_t size;
    Stop *stops;
} Route;

int route_init(Route *r);
int route_free(Route *r);
int route_print(const Route *r);
int route_push(Route *r, Stop s);
int route_filter(Route *r);

typedef struct Routes {
    size_t cap;
    size_t size;
    Route *ptr;
} Routes;

int routes_init(Routes *r);
int routes_free(Routes *r);
int routes_print(const Routes *r);
int routes_parse_file(Routes *r, FILE *f);
int routes_push(Routes *r, Route s);
int routes_push_with_id(Routes *r, const char *id);
int routes_filter(Routes *r);

// finds the extremum value. if max is true - maximum else minumum
int routes_find_ext_routes_cnt(Route **res, const Routes *r, bool max);
int routes_find_ext_path_len(Route **res, const Routes *r, bool max);
int routes_find_ext_route_len(Route **res, const Routes *r, bool max);
int routes_find_ext_stop_dur(Route **res, const Routes *r, bool max);
int routes_find_ext_stop_total_dur(Route **res, const Routes *r, bool max);

typedef int (*mask)(int);
int parse_field_uint(unsigned long *res, const char **start, mask m);
int parse_field_float(float *res, const char **start, mask m);
int parse_field_str(char **res, const char **start, mask m);
int parse_field_time(tm *res, const char **start, mask m);

#endif
