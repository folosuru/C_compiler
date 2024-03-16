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
    TOKEN_EOF
} TokenType;


typedef struct Token Token;
struct Token {
    TokenType type;
    Token* next;
    int value;
    char* string;
    int length;
};
void error(char* fmt, ...);
void error_at(char* source, char* point, char* fmt, ...);

extern Token* now_token;
extern char* input;

int calc_total_offset();
#endif