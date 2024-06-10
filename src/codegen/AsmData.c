#include "AsmData.h"

operand* redister_operand_wellknown[16][4];
 List_iter* output_asm_list;

operand* create_operand_redister(redister_word word, int size) {
    if (redister_operand_wellknown[word][size] != 0) {
        return redister_operand_wellknown[word][size];
    }
    redister_operand_wellknown[word][size] = calloc(1, sizeof(operand));
    redister_operand_wellknown[word][size]->type = value_preserved;
    redister_operand_wellknown[word][size]->value.redister.word = word;
    redister_operand_wellknown[word][size]->value.redister.size = size;
    return redister_operand_wellknown[word][size];
}
operand* create_operand_text(char* write_str) {
    operand* result = calloc(1, sizeof(operand));
    result->type = value_text;
    result->value.text = write_str;
    return result;
}
operand* create_operand_text_fmt(char* write_str, ...) {
    operand* result = calloc(1, sizeof(operand));
    char* str = calloc(10, sizeof(char));
    va_list ap;
    va_start(ap, write_str);

    int write_size =  vsnprintf(str, 10, write_str, ap);
    if (write_size > 10) {
        free(str);
        str = calloc(write_size + 1, sizeof(char));
        vsnprintf(str, write_size + 1, write_str, ap);
    }
    result->type = value_text;
    result->value.text = str;
    va_end(ap);
    return result;
}

operand* create_operand_num(int value) {
    operand* result = calloc(1, sizeof(operand));
    result->type = value_num;
    result->value.number = value;
    return result;
}
void create_asm_statement_enum(Asm_word word, operand* op1, operand* op2) {
    Asm_statement* result = calloc(1, sizeof(Asm_statement));
    result->instruction.type = value_preserved;
    result->instruction.value.preserved = word;
    result->instruction.need_indent = true;
    result->operand1 = op1;
    result->operand2 = op2;
    join_asm_statement(result);
}
void create_asm_statement_text(char* word, operand* op1, operand* op2) {
    Asm_statement* result = calloc(1, sizeof(Asm_statement));
    result->instruction.type = value_text;
    result->instruction.value.text = word;
    result->instruction.need_indent = true;
    result->operand1 = op1;
    result->operand2 = op2;
    join_asm_statement(result);
}

void create_asm_statement_directive(char* directive) {
    Asm_statement* result = calloc(1, sizeof(Asm_statement));
    result->instruction.type = value_text;
    result->instruction.value.text = directive;
    result->instruction.need_indent = false;
    result->operand1 = 0;
    result->operand2 = 0;
    join_asm_statement(result);
}

void create_asm_statement_directive_fmt(char* write_str, ...) {
    char* str = calloc(10, sizeof(char));
    va_list ap;
    va_start(ap, write_str);

    int write_size =  vsnprintf(str, 10, write_str, ap);
    if (write_size > 10) {
        free(str);
        str = calloc(write_size + 1, sizeof(char));
        vsnprintf(str, write_size + 1, write_str, ap);
    }
    create_asm_statement_directive(str);
}

void join_asm_statement(Asm_statement* statement) {
    List_iter* result = add_reverse_array_upd(&output_asm_list);
    result->data = statement;
}

void print_asm_statement(Asm_statement* stm) {
    if (stm->instruction.need_indent == true) {
        printf("  ");
    }
    if (stm->instruction.type == value_text) {
        printf("%s  ", stm->instruction.value.text);
    } else {
        printf("%s  ", getAsm_instruction_word(stm->instruction.value.preserved));
    }

    if (stm->operand1 != 0) {
        if (stm->operand1->type == value_text) {
            printf("%s", stm->operand1->value.text);
        } else if (stm->operand1->type == value_preserved) {
            printf("%s", getRedisterNameByPrefix(&stm->operand1->value.redister));
        } else {
            printf("%d", stm->operand1->value.number);
        }
    }
    if (stm->operand2 != 0) {
        if (stm->operand2->type == value_text) {
            printf(", %s", stm->operand2->value.text);
        } else if (stm->operand2->type == value_preserved) {
            printf(", %s", getRedisterNameByPrefix(&stm->operand2->value.redister));
        } else {
            printf(", %d", stm->operand2->value.number);
        }
    }
    printf("\n");
}

void print_asm_statement_list() {
    if (output_asm_list != 0) {
    for (List_iter* current = output_asm_list->index->start; current != 0; current = current->next ) {
        Asm_statement* current_var = current->data;
        if (current_var != 0) {
            print_asm_statement(current_var);
        }
    }
    }
}