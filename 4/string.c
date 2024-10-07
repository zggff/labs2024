#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

int main(void) {
    String a = {0};
    String b = {0};
    String c = {0};
    string_from_str(&a, "hello, world!");
    string_from_str(&b, "\n");
    string_from_str(&c, "abcdefg");

    String d = {0};
    string_copy(&d, &a);
    printf("copy: [%s]\n", d.ptr);
    string_free(&d);

    String *e;
    string_create_copy(&e, &a);
    printf("copy into malloc: [%s]\n", e->ptr);
    string_free(e);
    free(e);

    string_concatenate(&a, &b);
    string_concatenate(&a, &c);
    printf("concatenated [%s]\n", a.ptr);

    string_free(&a);
    string_free(&b);
    string_free(&c);

    string_from_str(&a, "12345679");
    string_from_str(&b, "12345678");
    string_from_str(&c, "123456780");
    string_from_str(&d, "123456780");
    printf("\n");
    printf("a = [%s]\n", a.ptr);
    printf("b = [%s]\n", b.ptr);
    printf("c = [%s]\n", c.ptr);
    printf("d = [%s]\n", c.ptr);
    int res;
    string_compare(&res, &a, &b);
    printf("cmp(a, b) = %d\n", res);
    string_compare(&res, &a, &c);
    printf("cmp(a, c) = %d\n", res);
    string_compare(&res, &a, &a);
    printf("cmp(a, a) = %d\n", res);
    string_compare(&res, &c, &d);
    printf("cmp(c, d) = %d\n", res);
    bool eq;
    string_equal(&eq, &a, &b);
    printf("eq(a, b) = %d\n", eq);
    string_equal(&eq, &c, &d);
    printf("eq(c, d) = %d\n", eq);

    return 0;
}
