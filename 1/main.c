#include <stdio.h>

int convert_to_base2r_and_print_rec_(unsigned long n, int power) {
    if (!n)
        return 0;

    convert_to_base2r_and_print_rec_(n >> power, power);
    unsigned long res = 0;
    unsigned long my_power = 1 << power >> 1;
    while (my_power) {
        res = res | (n & my_power);
        my_power = my_power >> 1;
    }
    printf("%ld ", res);
    return 0;
}

int convert_to_base2r_and_print_rec(unsigned long n, unsigned int power) {
    if (!power || power >> 6)
        return 1;
    convert_to_base2r_and_print_rec_(n, power);
    printf("\n");
    return 0;
}

int convert_to_base2r_and_print(unsigned long n, int power) {
    if (!power || power >> 6)
        return 1;
    if (!n) {
        printf("0\n");
        return 0;
    }
    unsigned long n2 = n;
    unsigned long r = 1;
    while (n) {
        n = n >> power;
        r = r << 1;
    }
    r = r >> 1;
    while (r) {
        n = n2;
        int r2 = r >> 1;
        while (r2) {
            n = n >> power;
            r2 = r2 >> 1;
        }
        unsigned long res = 0;
        unsigned long my_power = 1 << power >> 1;
        while (my_power) {
            res = res | (n & my_power);
            my_power = my_power >> 1;
        }
        printf("%ld ", res);
        r = r >> 1;
    }
    printf("\n");
    return 0;
}

int main(void) {
    int n = 2150;
    printf("[%x]\n", n);
    convert_to_base2r_and_print(0, 1);
    convert_to_base2r_and_print_rec(0, 1);
    convert_to_base2r_and_print(n, 1);
    convert_to_base2r_and_print_rec(n, 1);
    convert_to_base2r_and_print(n, 3);
    convert_to_base2r_and_print_rec(n, 3);
    convert_to_base2r_and_print(n, 4);
    convert_to_base2r_and_print_rec(n, 4);
    convert_to_base2r_and_print(n, 10);
    convert_to_base2r_and_print_rec(n, 10);
    return 0;
}
