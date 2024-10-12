#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

typedef int (*handle)(Post *p, char *s);

int handle_add(Post *p, char *s) {
    int r;
    Mail m;
    char *start = s;
    if ((r = mail_from_string(&m, &s)) != S_OK) {
        fprintf(stderr, "ERROR: failed to parse [%s] as mail\n", start);
        return r;
    }
    if ((r = post_add(p, m)) != S_OK) {
        fprintf(stderr, "ERROR: failed to add mail to post\n");
        return r;
    }
    return S_OK;
}

int handle_remove(Post *p, char *s) {
    int r;
    char *start = s;
    unsigned i;
    if ((r = parse_field_uint(&i, &s, " ")) != S_OK) {
        fprintf(stderr, "ERROR: failed to parse [%s] as unsigned\n", start);
        return r;
    }
    if ((r = post_remove(p, i)) != S_OK) {
        fprintf(stderr, "ERROR: failed to remove mail to post\n");
        return r;
    }
    return S_OK;
}

int handle_sort(Post *p, char *s) {
    (void)s;
    return post_sort(p);
}

int handle_print(Post *p, char *s) {
    (void)s;
    return post_print(p);
}

int handle_delivered_status(Post *p, bool delivered) {
    Post deliv = {0};
    check(post_init(&deliv), {});
    check(post_filter_by_delivery_status(&deliv, p, delivered), {});
    post_print(&deliv);
    free(deliv.mail);
    // post_free(&deliv);
    return S_OK;
}

int handle_delivered(Post *p, char *s) {
    (void)s;
    return handle_delivered_status(p, true);
}

int handle_notdelivered(Post *p, char *s) {
    (void)s;
    return handle_delivered_status(p, false);
}

int handle_id(Post *p, char *s) {
    String id = {0};
    int r;
    if ((r = parse_field_str(&id, &s, " ")) != S_OK) {
        fprintf(stderr, "ERROR: failed to parse [%s] as unsigned\n", s);
        return r;
    }
    bool eq = false;
    for (size_t i = 0; i < p->size; i++) {
        string_equal(&eq, &p->mail[i].id, &id);
        if (!eq)
            continue;
        time_t now = time(NULL);
        time_t t;
        parse_time(&t, &p->mail[i].receive);

        if (difftime(t, now) <= 0)
            printf("mail was delivered\n");
        else
            printf("mail was not delivered\n");
        break;
    }
    string_free(&id);
    if (!eq) {
        printf("mail not found\n");
    }

    return S_OK;
}

int main(void) {
    size_t line_len = 0;
    char *line = NULL;
    Post p = {0};
    if (post_init(&p) != S_OK) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        return 1;
    }
    const char *ops[] = {"Add", "Remove",    "Sort",        "Print",
                         "Id",  "Delivered", "NotDelivered"};
    handle handles[] = {handle_add,         handle_remove, handle_sort,
                        handle_print,       handle_id,     handle_delivered,
                        handle_notdelivered};
    while (true) {
        String op = {0};
        int n = getline(&line, &line_len, stdin);
        if (n <= 0)
            break;
        n--;
        line[n] = 0;

        char *s = line;
        parse_field_str(&op, &s, " ");
        bool res = false;
        for (size_t i = 0; i < (sizeof(handles) / sizeof(handles[0])); i++) {
            string_equal_str(&res, &op, ops[i]);
            if (res) {
                handles[i](&p, s);
                break;
            }
        }
        if (!res) {
            fprintf(stderr, "ERROR: unknown operation: [%s]\n", op.ptr);
            fprintf(stderr, "supported operations: {");
            for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
                fprintf(stderr, "%s, ", ops[i]);
            }
            fprintf(stderr, "}\n");
        }
        string_free(&op);
    }
    if (line)
        free(line);
    post_free(&p);
    return 0;
}
