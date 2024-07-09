#ifndef CODEGEN_PRINTASM_H
#define CODEGEN_PRINTASM_H
#include "Asm_data_def.h"
#include "../Node/Node.h"

char* getRedisterName(redister_word word, int size_byte);
char* getRedisterNameByPrefix(struct redister_prefix* prefix);
char* getAsm_instruction_word(Asm_word word);
void print_function_def(function_def*);
void print_global_var_def(Globalvar_def*);
void print_text_literal_def(string_literal_data*);

void print_push_register(redister_word word);
void print_push_number(int num);

void print_pop(redister_word word);
void print_mov_redister(redister_word to, redister_word from);

void align_rsp(int after_push_count);
void un_align_rsp(int before_push_count) ;
void print_args_push(function_def* function);
char* get_action_size_prefix(int byte_size);
redister_word get_args_place_for(int arg_index) ;
#endif  // CODEGEN_PRINTASM_H
