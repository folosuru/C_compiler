#ifndef CODEGEN_ASMDATA_H
#define CODEGEN_ASMDATA_H
#include "printAsm.h"
#include "Asm_data_def.h"
#include "../util/array_util.h"


operand* create_operand_redister(redister_word, int size);
operand* create_operand_text(char*);
operand* create_operand_text_fmt(char* write_str, ...);
operand* create_operand_num(int);
void create_asm_statement_enum(Asm_word, operand*, operand*);
void create_asm_statement_text(char*, operand*, operand*);
void create_asm_statement_directive(char*);
void create_asm_statement_directive_fmt(char*, ...);
void join_asm_statement(Asm_statement*);
void print_asm_statement_list();
#endif  // CODEGEN_ASMDATA_H

