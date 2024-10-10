#include "lib.h"

int main(int argc, const char *argw[]) {
    if (argc < 1) {
        fprintf(stderr, "ERROR: no input file provided\n");
        return 1;
    }
    FILE *f = fopen(argw[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file: [%s]\n", argw[1]);
        return 1;
    }

    fclose(f);
    return 0;
}
