void printf();
void exit();

int test_assert(int result, int expect, char* test_text ) {
    if (result != expect) {
        printf("%s: %d expected, but %d\n", test_text, expect, result);
        exit(1);
    }
    printf("%s => %d\n", test_text, result);
    return 1;
}


int main() {
    printf("Hello, world!\n");
    test_assert(1+1, 2, "1+1");
    test_assert(4*4, 16, "8*8");
    test_assert(5==5, 1, "5==5;");
    test_assert(5!=5, 0, "5!=5;");
    test_assert(3==5, 0, "3==5;");
    test_assert(3 < 5, 1, "3 < 5;");
    test_assert(3 <= 5, 1, "3 <= 5;");
    test_assert(-5 < 7, 1, "-5 < 7;");
    printf("test ok\n");
    return 1;
}