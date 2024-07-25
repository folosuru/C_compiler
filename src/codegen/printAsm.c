#include "printAsm.h"
#include "AsmData.h"
#include "../util/string_util.h"
#include <stdio.h>
int stack_depth;
int align_value;
int function_offset_var;

char* redister_word_str[][4] = {
//   64bit  32bit   16bit   8bit
    {"rax", "eax",  "ax",   "al"  },
    {"rbx", "ebx",  "bx",   "bl"  },
    {"rcx", "ecx",  "cx",   "cl"  },
    {"rdx", "edx",  "dx",   "dl"  },
    {"rsi", "esi",  "si",   "sil" },
    {"rdi", "edi",  "di",   "dil" },
    {"rbp", "ebp",  "bp",   "bpl" },
    {"rsp", "esp",  "sp",   "spl" },
    {"r8 ", "r8d",  "r8w" , "r8b" },
    {"r9 ", "r9d",  "r9w" , "r9b" },
    {"r10", "r10d", "r10w",	"r10b"},
    {"r11", "r11d", "r11w",	"r11b"},
    {"r12", "r12d", "r12w",	"r12b"},
    {"r13", "r13d", "r13w",	"r13b"},
    {"r14", "r14d", "r14w",	"r14b"},
    {"r15", "r15d", "r15w",	"r15b"}
};
char* getRedisterName(redister_word word, int size_byte) {
    switch (size_byte){
    case 8:
        return redister_word_str[word][0];
    case 4:
        return redister_word_str[word][1];
    case 2:
        return redister_word_str[word][2];
    case 1:
        return redister_word_str[word][3];
    default:
        return "UNKNOWN TYPE SIZE";
    }
}

char* getRedisterNameByPrefix(struct redister_prefix* prefix) {
    return getRedisterName(prefix->word, prefix->size);
}

char* asm_instruction_word_str[] = {
    "push",
    "pop",
    "mov",
    "movzx",
    "movsx",
    "movzb",
    "add",
    "sub",
    "imul",
    "idiv",
    "cmp",
    "je",
    "jmp",
    "cqo",
    "sete",
    "setne",
    "setl",
    "setle",

};
char* getAsm_instruction_word(Asm_word word) {
    return asm_instruction_word_str[word];
}

void print_push_register(redister_word word) {
    //printf("  push %s\n", getRedisterName(word, 8));
    create_asm_statement_enum(push, create_operand_redister(word, 8), 0);
    stack_depth++;
}

void print_push_number(int num) {
    //printf("  push %d\n", num);
    stack_depth++;
    create_asm_statement_enum(push, create_operand_num(num), 0);
}

void print_pop(redister_word word) {
    //printf("  pop %s\n", getRedisterName(word, 8));
    create_asm_statement_enum(pop, create_operand_redister(word, 8), 0);
    stack_depth--;
}

void print_mov_redister(redister_word to, redister_word from) {
    printf("  mov %s, %s\n", getRedisterName(to, 8), getRedisterName(from, 8));
}


void align_rsp(int after_push_count) {
    align_value = (function_offset_var % 16) + (((stack_depth + after_push_count) % 2)*8);
    if (align_value != 0) {
        align_value = 16 - align_value;
    }

    if (align_value != 0) {
        //printf("  sub rsp, %d\n", align_value);
        create_asm_statement_enum(sub, create_operand_redister(rsp, 8), create_operand_num(align_value));
    } else {
        align_value = 0;
    }
}

int sub_redister_arg_count(int val) {
    if (val >= 4) {
        return val - 4;
    } else {
        return 0;
    }
}

void un_align_rsp(int before_push_count) {
    stack_depth = stack_depth - before_push_count;
    if (sub_redister_arg_count(before_push_count) * 8 + align_value != 0) {
//        printf("  add rsp, %d\n", ( sub_redister_arg_count(before_push_count)  * 8) + align_value);
        create_asm_statement_enum(add, create_operand_redister(rsp, 8), create_operand_num(( sub_redister_arg_count(before_push_count)  * 8) + align_value));
    }
}

redister_word get_args_place_for(int arg_index) {
    #ifdef ABI_LINUX
    switch (arg_index) {
    case 1: {
        return rdi;
        break;
    }
    case 2: {
        return rsi;
        break;
    }
    case 3: {
        return rdx;
        break;
    }
    case 4: {
        return rcx;
        break;
    }
    case 5: {
        return r8;
        break;
    }
    case 6: {
        return r9;
        break;
    }
    default:
        break;
    }
    #else 
    switch (arg_index) {
    case 1: {
        return rcx;
        break;
    }
    case 2: {
        return rdx;
        break;
    }
    case 3: {
        return r8;
        break;
    }
    case 4: {
        return r9;
        break;
    }
    default:
        break;
    }
    #endif
    return stack;
}

void print_function_def(function_def* function) {
    if (!function->program) {
        return;
    }
    stack_depth = 0;
    //printf(".text\n");
    create_asm_statement_directive(".text");
    //printf("%s:\n", str_trim(function->name, function->name_length));
    create_asm_statement_directive_fmt("%s:", str_trim(function->name, function->name_length));
    print_push_register(rbp);
    //printf("  mov rbp, rsp\n");
    create_asm_statement_enum(mov, create_operand_redister(rbp,8), create_operand_redister(rsp,8));

    function_offset_var = calc_total_offset(function);
    //printf("  sub rsp, %d\n", function_offset_var);
    create_asm_statement_enum(sub, create_operand_redister(rsp, 8), create_operand_num(function_offset_var));
    print_args_push(function);
    Program* program = function->program;
    while (program != 0) {
        if (program->node->type == NODE_DO_NOTHING) {
            program = program->next;
            continue;
        }
        calc(program->node);
        program = program->next;
    }
    //printf("  mov rsp, rbp\n");
    create_asm_statement_enum(mov, create_operand_redister(rsp, 8), create_operand_redister(rbp, 8));
    //printf("  pop rbp\n");
    print_pop(rbp);
    //printf("  ret\n");
    create_asm_statement_text("ret", 0,0);
}

void print_global_var_def(Globalvar_def* def) {
    //printf(".bss\n");
    create_asm_statement_directive(".bss");
    //printf("%s:\n", def->name);
    create_asm_statement_directive_fmt("%s:", def->name);
    //printf("  .zero %d\n", calc_var_size(def->type));
    create_asm_statement_directive_fmt("  .zero %d", calc_var_size(def->type));
}

void print_text_literal_def(string_literal_data* data) {
    create_asm_statement_directive(".text");
    create_asm_statement_directive_fmt(".Ltext%d:", data->id);
    create_asm_statement_directive_fmt("  .string %s", str_trim(data->text, data->literal_length + 1));
}

void print_args_push(function_def* function) {
    for (int i = 1;function->args_count >= i; i++) {
        redister_word arg_place = get_args_place_for(i);
        if (arg_place != stack) {
            //printf("  mov [ rbp + %d ], %s\n", (i+1)*8 ,getRedisterName(arg_place, 8));
            create_asm_statement_enum(mov, 
                create_operand_text_fmt("[ rbp + %d ]", (i+1)*8),
                create_operand_redister(arg_place, 8));
        } else {
            break;
        }
    }
}
char* get_action_size_prefix(int byte_size) {
    switch (byte_size) {
        case 1: return "BYTE";
        case 2: return "WORD";
        case 4: return "DWORD";
        case 8: return "QWORD";
        default: return  "UNKNOWN TYPE SIZE";
    }
}