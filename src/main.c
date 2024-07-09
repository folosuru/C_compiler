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
#include "codegen/AsmData.h"
#include "optimize/optimize.h"
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
        asm_label_def* function = getFunction();
        if (function == 0) {
            break;
        }
        if (function->func != 0) {
            print_function_def(function->func);
        }
        if (function->variable != 0) {
            print_global_var_def(function->variable);
        }
    }
    printf("# ----\n");
    //print_asm_statement_list();
    //optimize_asm(output_asm_list->index);
    printf("# ----\n");
    print_asm_statement_list();
    return 0;
}