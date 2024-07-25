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
#include "util/file_load.h"
char* input;

int main(int argc , char **argv) {
    if (argc != 2){
        fprintf(stderr , "hikisu");
        return 1;
    }
    input = file_load_buf(argv[1]);
    
    struct tokenize_result* tokenize_result = tokenize(input);
    now_token = tokenize_result->token;
    if (tokenize_result->string_literal) {
    List_iter* current_text = tokenize_result->string_literal->index->start;
    while (true) {
        if (current_text == 0) {
            break;
        }
        print_text_literal_def(current_text->data);
        current_text = current_text->next;
    }
    }

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    while (true) {
        asm_label_def* function = getFunction();
        if (function == 0) {
            break;
        }
        if (function->nothing) {
            continue;
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