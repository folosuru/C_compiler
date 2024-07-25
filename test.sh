#!/bin/bash

assert_func() {
  expected="$1"
  input="$2"
  ./build/compiler "$input"  > ./test/tmp.s
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



cmake -S . -B ./build/ > /dev/null
cmake --build ./build/
cmake_result="$?"
if [ "$cmake_result" != "0" ]; then
  echo 'build failed'
  exit 1
fi

assert_func 0 'test/test.c'
exit 0