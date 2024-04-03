#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/array_util.h"
typedef enum {
    TOKEN_PREFIX,
    TOKEN_IDENTIFY,
    TOKEN_NUM,
    TOKEN_PRESERVED_WORD,
    TOKEN_STRING,
    TOKEN_EOF
} TokenType;

typedef struct {
    char* text;
    int literal_length;
    int string_length;
    int id;
} string_literal_data;


typedef struct Token Token;
struct Token {
    TokenType type;
    Token* next;
    int value;
    char* string;
    int length;
    union {
        string_literal_data* str_data;
    } data;
};


struct tokenize_result {
    Token* token;
    List_iter* string_literal;
};
struct tokenize_result* tokenize(char* p);

void error(char* fmt, ...);
void error_at(char* source, char* point, char* fmt, ...);

extern Token* now_token;
extern char* input;

int calc_total_offset();
#endif