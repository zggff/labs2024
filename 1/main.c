#include <stdio.h>

int convert_to_base2r_and_print_(unsigned long n, int power) {
    if (!n)
        return 0;

    convert_to_base2r_and_print_(n >> power, power);
    unsigned long res = 0;
    unsigned long my_power = 1 << power >> 1;
    while (my_power) {
        res = res | (n & my_power);
        my_power = my_power >> 1;
    }
    printf("%ld ", res);
    return 0;
}

int convert_to_base2r_and_print(unsigned long n, unsigned int power) {
    if (!power || power >> 6)
        return 1;
    convert_to_base2r_and_print_(n, power);
    printf("\n");
    return 0;
}

int main(void) {
    int n = 2050;
    printf("[%x]\n", n);
    convert_to_base2r_and_print(n, 1);
    convert_to_base2r_and_print(n, 3);
    convert_to_base2r_and_print(n, 4);
    convert_to_base2r_and_print(n, 10);
    return 0;
}
