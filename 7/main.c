#include "lib.h"

int main(int argc, const char *argw[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: no input file provided\n");
        return 1;
    }
    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[1]);
        return 1;
    }

    Db d = {0};
    db_init(&d);
    db_read_file(&d, f);
    db_print(&d);
    db_free(&d);

    fclose(f);
    return 0;
}
