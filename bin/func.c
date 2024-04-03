#include "stdio.h"
#include "stdlib.h"

int func(int a, int b) {
    printf("%d\n", a + b);
    return 0;
}

int print_num(int a) {
    printf("%d\n",a);
    return 0;
}

int add_func(int a, int b) {
    return a+b;
}

int print_char_p(char* p) {
    printf(p);
    return 0;
}

int* alloc_args(int arg1, int arg2, int arg3) {
    int* result = calloc(3, sizeof(int));
    result[0] = arg1;
    result[1] = arg2;
    result[2] = arg3;
    return result;
}