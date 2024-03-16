#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GlobalVariable.h"
#include "compiler.h"
#include "Node/Node.h"
#include "codegen/printAsm.h"
char* input;


int main(int argc , char **argv) {
    if (argc != 2){
        fprintf(stderr , "hikisu");
        return 1;
    }
    input = argv[1];
    now_token = tokenize(input);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    while (true) {
        function_def* function = getFunction();
        if (function == 0) {
            break;
        }
        print_function_def(function);
    }
    return 0;
}