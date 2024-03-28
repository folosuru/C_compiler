#include "print_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
void error_at(char* source, char* point, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = point - source;
    fprintf(stderr, "%s\n", source);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
void error_token(Token* token, char* msg,  ...) {
    extern char* input;
    char* source = input;
    va_list ap;
    va_start(ap, msg);
    int pos = token->string - source;
    fprintf(stderr, "%s\n", source);
    fprintf(stderr, "%*s", pos - 1, " ");
    fprintf(stderr, "^");
    for (int i = 0; i < token->length - 1; i++) {
        fprintf(stderr, "~");
    }
    fprintf(stderr, "\n%*s", pos, " ");
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    exit(1);
}
