#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

typedef int (*handle)(Routes *r);

int handle_print(Routes *r) {
    return routes_print(r);
}

#define FIND_ROUTE(FUNC, MAX)                                                  \
    {                                                                          \
        Route *ro = NULL;                                                      \
        FUNC(&ro, r, MAX);                                                     \
        if (ro)                                                                \
            printf("%s\n", ro->id);                                            \
        else                                                                   \
            printf("no route found\n");                                          \
        return S_OK;                                                           \
    }

int handle_most_routes(Routes *r) {
    FIND_ROUTE(routes_find_ext_routes_cnt, true);
}
int handle_least_routes(Routes *r) {
    FIND_ROUTE(routes_find_ext_routes_cnt, false);
}
int handle_longest_path(Routes *r) {
    FIND_ROUTE(routes_find_ext_path_len, true);
}
int handle_shortest_path(Routes *r) {
    FIND_ROUTE(routes_find_ext_path_len, false);
}
int handle_longest_route(Routes *r) {
    FIND_ROUTE(routes_find_ext_route_len, true);
}
int handle_shortest_route(Routes *r) {
    FIND_ROUTE(routes_find_ext_route_len, false);
}
int handle_longest_stop(Routes *r) {
    FIND_ROUTE(routes_find_ext_stop_dur, true);
}
int handle_shortest_stop(Routes *r) {
    FIND_ROUTE(routes_find_ext_stop_dur, false);
}
int handle_max_wait(Routes *r) {
    FIND_ROUTE(routes_find_ext_stop_total_dur, true);
}

int main(int argc, const char *argw[]) {
    Routes r = {0};
    check(routes_init(&r), {});
    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(argw[i], "r");
        if (!f) {
            fprintf(stderr, "ERROR: failed to open file [%s]\n", argw[i]);
            return 1;
        }
        check(routes_parse_file(&r, f),
              fprintf(stderr, "ERROR: failed to parse file [%s]\n", argw[i]));
        fclose(f);
    }
    routes_filter(&r);

    const char *ops[] = {
        "Print",        "MostRoutes",   "LeastRoutes",   "LongestPath",
        "ShortestPath", "LongestRoute", "ShortestRoute", "LongestStop",
        "ShortestStop", "MaxWait",
    };
    const handle handles[] = {handle_print,          handle_most_routes,
                              handle_least_routes,   handle_longest_path,
                              handle_shortest_path,  handle_longest_route,
                              handle_shortest_route, handle_longest_stop,
                              handle_shortest_stop,  handle_max_wait};

    size_t line_len;
    char *line;
    while (true) {
        int n = getline(&line, &line_len, stdin);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;

        bool found = false;
        for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
            if (strcmp(line, ops[i]) == 0) {
                handles[i](&r);
                found = true;
            }
        }
        if (!found) {
            fprintf(stderr, "ERROR: unknown operation [%s]\n", line);
            fprintf(stderr, "supported operations: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
                fprintf(stderr, "%s, ", ops[i]);
            }
            fprintf(stderr, "}\n");
        }
    }

    if (line)
        free(line);

    routes_free(&r);
    return 0;
}
