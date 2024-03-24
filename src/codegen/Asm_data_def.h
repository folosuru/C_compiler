#ifndef CODEGEN_ASM_DATA_DEF_H
#define CODEGEN_ASM_DATA_DEF_H
#include "../util/array_util.h"
#include <stdbool.h>
typedef enum  {
    rax,
    rbx,
    rcx,
    rdx,
    rsi,
    rdi,
    rbp,
    rsp,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,
    stack
} redister_word;

struct redister_prefix {
    redister_word word;
    int size;
};

typedef enum {
    push,
    pop,
    mov,
    movzx,
    movsx,
    movzb,
    add,
    sub,
    imul,
    idiv,
    cmp,
    je,
    jmp,
    cqo,
    sete,
    setne,
    setl,
    setle
} Asm_word;

typedef enum {
    value_preserved,
    value_text,
    value_num
} keyword_value_type;

typedef struct {
    keyword_value_type type;
    union {
        char* text;
        struct redister_prefix redister;
        int number;
    } value;
} operand;

struct asm_instructions {
    keyword_value_type type;
    union {
        Asm_word preserved;
        char* text;
    } value;
    bool need_indent;
};

typedef struct Asm_statement Asm_statement ;
struct Asm_statement {
    struct asm_instructions instruction;
    operand* operand1;
    operand* operand2;
};

extern List_iter* output_asm_list;

#endif
