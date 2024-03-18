#!/bin/bash

assert_func() {
  expected="$1"
  input="$2"
  ./build/compiler "$input" > ./test/tmp.s
  result="$?"
  if [ "$result" != "0" ]; then 
    echo "compile failed: return $result";
    exit 1
  fi
  cc -o ./test/res ./test/tmp.s ./bin/libfunc.a -static
  ./test/res
  actual="$?"
  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}
assert() {
  assert_func "$1" "int main() {$2}"
}



cmake -S . -B ./build/
cmake --build ./build/
cmake_result="$?"
if [ "$cmake_result" != "0" ]; then
  echo 'build failed'
  exit 1
fi

<< TEST_PASS
assert 0 '0;'
assert 42 '42;'
assert 64 '8*8;'
assert 11 '8+8-5;'
assert 3 'int a;a = -57; return 60 + a;'
assert 13 '(1*4)+(8+1);'
assert 1 '5==5;'
assert 0 '5!=5;'
assert 0 '3==5;'
assert 1 '3 < 5;'
assert 1 '3 <= 5;'
assert 1 '-5 < 7;'
assert 7 'int b=5;b = 7;b;'
assert 6 'int a;int b;int c; a=b=c=2;return a+b+c;'
assert 16 'int hoge;int fuga;int hoya; hoge = 13;fuga = 24;hoya = 16;return hoya;'
assert 24 'int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return fuga;'
assert 12 'int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return hoge;'
assert 24 'int hoge;int fuga;  hoge = 13;fuga = 24;hoge = 12;return fuga;'
assert 24 'int hoge;int fuga;int piyo; int mojo;   mojo = 78;hoge = 13;fuga = 24;hoge = 12;piyo = 27; return fuga;'
assert 13 'int bb = 13;return bb;'
assert 24 'return 24;'
assert 182 'int a=182;return a;'
assert 17 'int a=15; a= a+2;return a;'
assert 7 'if (0 == 0) if (7 == 7) return 7; else return 5; else return 28;'
assert 8 'int b = 8;for (int c=4;b < 8;b = b+1) c = 2;return b;'
assert 20 'int b = 0;for (int  c=4 ;b < 8;b = b+1) c = c + 2;return c;'
assert 7 '{return 7;}return 5;'
assert 13 '{int a = 13;3; {return a;}return 3;}return 5;'
assert 5 'if (7 == 7) {return 5;}'
assert 5 'if (7 == 7) if (13==13) return 5;'
assert 5 'if (7 == 7) {int a = 13;if (13==13) {a = 3;return 5;}}return 8;'
assert 3 'if (7 == 7) {int a = 13;if (a == 13) {a = 3;return a;}} return 7;'
assert 5 'return add_func(2,3);'
assert_func 2 'int sub() {int a=13; return a;} int main() {int a=2; int b = sub(); return a;}'
assert_func 5 'int sub(int a,int b){return a+b;} int main(){return sub(3,2);}'
assert_func 7 'int sub(int a,int b,int c,int d,int e,int f,int g){return a+b+c+d+e+f+g;} int  main(){return sub(1,1,1,1,1,1,1);}'
assert_func 0 'int res(int var, int old_var) { print_num(var+old_var); if (var > 1000) {return 0;} return res(var+old_var , var);} int main() {res(1,0); return 0;}'
assert 4 'int* z; int s; s = 4; return s;'
assert 4 'int s;int* z = 1; return z;'
assert 5 'int*  y;int x=5;y = &x;return *y;'
assert 8 'int** x = 0;x = x + 1; return x;' 
assert 7 'int* y; int x; x=5;y = &x;  *y = 7;  return *y;'
assert 5 'int x=5;int* y = &x; int** z = &y;  return **z;'
assert_func 5 'int main(){int*** p; return 5;}'
assert 5 'if (sizeof(5) == 4) {int* p; if (sizeof(p) == 8) {return 5;}} return 0;'
assert 8 'return sizeof(int*);'
assert_func 0 'int foo(); int main(){return 0;}'
assert_func 6 'int* alloc_args(int arg1, int arg2, int arg3); int main() {int* p = alloc_args(1,2,3); return *(p) + *(p+1) + *(p+2);}'
assert_func 7 'int* alloc_args(int arg1, int arg2, int arg3); int main() {int* p = alloc_args(1,2,3); *(p+1) = 7; return *(p+1);}'


assert 5 'int var = 5;int* a = &var; return a[0];'
assert 7 'int a[8]; a[1] = 7; return a[1];'
assert 5 'int a[8][4]; a[2][3] = 5; return a[2][3];'
assert_func 3 'int* alloc_args(int arg1, int arg2, int arg3); int main() {int* p = alloc_args(1,2,3); p[2];}'
assert 3 'int a[2];*a = 1;*(a + 1) = 2;int *p;p = a;return *p + *(p + 1);'
TEST_PASS

assert 12 'char a; char b; b = 5; a = 7; return a+b;'
assert 144 'char a; char b; b = 200; a = 200; return a+b;'
assert 12 'char a[12]; return sizeof(a);'
assert 3 'char x[3];x[0] = -1;x[1] = 2;int y;y = 4;return x[0] + y;'
echo OK