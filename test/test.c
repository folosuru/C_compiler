void printf();
void exit();
void* calloc();


int g_var;

int test_assert(int result, int expect, char* test_text ) {
    if (result != expect) {
        printf("%s: %d expected, but %d\n", test_text, expect, result);
        exit(1);
    }
    printf("%s => %d\n", test_text, result);
    return 1;
}
int test_1() {
    int b=5;
    b = 7;
    return b;
}
 int test_2() {
    int a;
    int b;
    int c;
    a=18;
    b=9;
    c=3;
    a=5;
    return a+b+c;
 }
 int test_3() {
    if (1 == 1) {
        if (7 != 7) {
            return 7;
        } else {
            return 1;
        }
    }
    return 8;
 }
 int test_4() {
    int x=5;
    int* y = &x;
    int** z = &y;
    return **z;
 }
 int test_5(){
    int foo[5];
    foo[2] = 5;
    int* p = &(foo[2]);
    return *p;
 }

 int update_gvar(int a) {
    g_var = a;
    return 0;
 }

struct foo {
    char foooo;
    int a;
    char* hoge;
    int b;
};

struct foo_foo {
    struct foo f1;
    struct foo* f2;
};

int struct_access() {
    int other = 78416;

    int p = 7;
    struct foo val;
    val.b = 17;
    val.a = 18;
    
    if (other != 78416) {
        return 5;
    }
    if (p != 7) {
        return 4;
    }
    return val.a + val.b + p;
}

int struct_ptr() {
    struct foo* val = calloc(1, sizeof(struct foo));
    (*val).b = 42;
    val->a = 7;
    return (*val).b + val->a;
}

int more_struct() {
    struct foo_foo v;
    v.f1.a = 7;
    return v.f1.a;
}

struct t1 {
    char* str;
    char* string;
    int len;

};

int main() {
    printf("Hello, world!\n");
    test_assert(1+1, 2, "1+1");
    test_assert(4*4, 16, "8*8");
    test_assert(8/2, 4, "8/2");
    test_assert(5==5, 1, "5==5;");
    test_assert(5!=5, 0, "5!=5;");
    test_assert(3==5, 0, "3==5;");
    test_assert(3 < 5, 1, "3 < 5;");
    test_assert(3 <= 5, 1, "3 <= 5;");
    test_assert(-5 < 7, 1, "-5 < 7;");
    test_assert(test_1(), 7, "b=5;b=7;");
    test_assert(test_2(), 17, "a+b+c");
    test_assert(test_3(), 1, "if nest");
    test_assert(test_4(), 5, "ptr");
    test_assert(test_5(), 5, "array");
    g_var = 7;
    test_assert(g_var, 7, "global var");
    update_gvar(861);
    test_assert(g_var, 861, "global var 2");
    test_assert(sizeof(struct foo), 20, "sizeof struct foo");
    test_assert(struct_access(), 42, "struct access");
    test_assert(struct_ptr(), 49, "struct ptr");
    test_assert(more_struct(), 7, "struct 2");
    printf("test ok\n");
    return 1;
}