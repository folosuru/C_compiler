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
    struct {
        int b;
        char c;
    } inside;
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
    v.inside.b = 48;
    v.inside.c = 12;
    return v.f1.a;
}

struct t1 {
    char* str;
    char* string;
    int len;
};

int f() {
    return more_struct();
}


int test1() {
    42;
}
int test2() {
    8*8;
}
int test3() {
    8+8-5;
}
int test4() {
    int a;a = -57; return 60 + a;
}
int test5() {
    (1*4)+(8+1);
}
int test6() {
    5==5;
}
int test7() {
    5!=5;
}
int test8() {
    3==5;
}
int test9() {
    3 < 5;
}
int test10() {
    3 <= 5;
}
int test11() {
    -5 < 7;
}
int test12() {
    int b=5;b = 7;b;
}
int test13() {
    int a;int b;int c; a=b=c=2;return a+b+c;
}
int test14() {
    int hoge;int fuga;int hoya; hoge = 13;fuga = 24;hoya = 16;return hoya;
}
int test15() {
    int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return fuga;
}
int test16() {
    int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return hoge;
}
int test17() {
    int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return fuga;
}
int test18() {
    int hoge;int fuga;int piyo; int mojo;   mojo = 78;hoge = 13;fuga = 24;hoge = 12;piyo = 27; return fuga;
}
int test19() {
    int bb = 13;return bb;
}
int test20() {
    return 24;
}
int test21() {
    int a=182;return a;
}
int test22() {
    int a=15; a= a+2;return a;
}
int test23() {
    if (0 == 0) if (7 == 7) return 7; else return 5; else return 28;
}
int test24() {
    int b = 8;for (int c=4;b < 8;b = b+1) c = 2;return b;
}
int test25() {
    int b = 0;for (int  c=4 ;b < 8;b = b+1) c = c + 2;return c;
}
int test26() {
    {return 7;}return 5;
}
int test27() {
    {int a = 13;3; {return a;}return 3;}return 5;
}
int test28() {
    if (7 == 7) {return 5;}
}
int test29() {
    if (7 == 7) if (13==13) return 5;
}
int test30() {
    if (7 == 7) {int a = 13;if (13==13) {a = 3;return 5;}}return 8;
}
int test31() {
    if (7 == 7) {int a = 13;if (a == 13) {a = 3;return a;}} return 7;
}
int test32() {
    return add_func(2,3);
}
int test33() {
    int* z; int s; s = 4; return s;
}
int test34() {
    int s;int* z = 1; return z;
}
int test35() {
    int*  y;int x=5;y = &x;return *y;
}
int test36() {
    int** x = 0;x = x + 1; return x;
}
int test37() {
    int* y; int x; x=5;y = &x;  *y = 7;  return *y;
}
int test38() {
    int x=5;int* y = &x; int** z = &y;  return **z;
}
int test39() {
    if (sizeof(5) == 4) {int* p; if (sizeof(p) == 8) {return 5;}} return 0;
}
int test40() {
    return sizeof(int*);
}
int test41() {
    int var = 5;int* a = &var; return a[0];
}
int test42() {
    int a[8]; a[1] = 7; return a[1];
}
int test43() {
    int a[8][4]; a[2][3] = 5; return a[2][3];
}
int test44() {
    int a[2];*a = 1;*(a + 1) = 2;int *p;p = a;return *p + *(p + 1);
}
int test45() {
    int foo[5];foo[2] = 5; int* p = &(foo[2]);return *p;
}
int test46() {
    char a; char b; b = 5; a = 7; return a+b;
}

int test48() {
    char a[12]; return sizeof(a);
}
int test49() {
    char x[3];x[0] = -1;x[1] = 2;int y;y = 4;return x[0] + y;
}


int main() {
    printf("Hello, world!\n");
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


    test_assert(test1(), 42, "");
    test_assert(test2(), 64, "");
    test_assert(test3(), 11, "");
    test_assert(test4(), 3, "");
    test_assert(test5(), 13, "");
    test_assert(test6(), 1, "");
    test_assert(test7(), 0, "");
    test_assert(test8(), 0, "");
    test_assert(test9(), 1, "");
    test_assert(test10(), 1, "");
    test_assert(test11(), 1, "");
    test_assert(test12(), 7, "");
    test_assert(test13(), 6, "");
    test_assert(test14(), 16, "");
    test_assert(test15(), 24, "");
    test_assert(test16(), 12, "");
    test_assert(test17(), 24, "");
    test_assert(test18(), 24, "");
    test_assert(test19(), 13, "");
    test_assert(test20(), 24, "");
    test_assert(test21(), 182, "");
    test_assert(test22(), 17, "");
    test_assert(test23(), 7, "");
    test_assert(test24(), 8, "");
    test_assert(test25(), 20, "");
    test_assert(test26(), 7, "");
    test_assert(test27(), 13, "");
    test_assert(test28(), 5, "");
    test_assert(test29(), 5, "");
    test_assert(test30(), 5, "");
    test_assert(test31(), 3, "");
    test_assert(test32(), 5, "");
    test_assert(test33(), 4, "");
    test_assert(test34(), 4, "");
    test_assert(test35(), 5, "");
    test_assert(test36(), 8, "");
    test_assert(test37(), 7, "");
    test_assert(test38(), 5, "");
    test_assert(test39(), 5, "");
    test_assert(test40(), 8, "");
    test_assert(test41(), 5, "");
    test_assert(test42(), 7, "");
    test_assert(test43(), 5, "");
    test_assert(test44(), 3, "");
    test_assert(test45(), 5, "");
    test_assert(test46(), 12, "");
    test_assert(test48(), 12, "");
    test_assert(test49(), 3, "");


    printf("test ok\n");
    return 1;
}