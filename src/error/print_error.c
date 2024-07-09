#include "print_error.h"
#include "../util/string_util.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line_info {
    char* start;
    char* end;
    int number;
    int pos;
};

struct line_info get_line(char* source, char* point) {
    struct line_info result;
    result.number = 1;
    result.start = source;
    result.pos = 1;
    char* seek_current = source;
    while (seek_current < point) {
        if (*seek_current == '\n') {
            result.number++;
            result.pos = 1;
            result.start = seek_current + 1;
        }
        seek_current++;
        result.pos++;
    }
    result.end = result.start;
    while (*result.end != '\n') {
        result.end++;
    }
    return result;
}

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
    struct line_info line = get_line(source, point);
    fprintf(stderr, " | %s\n", str_trim(line.start, line.end - line.start));
    fprintf(stderr, " %*s", line.pos, " ");
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

    struct line_info line = get_line(source, token->string);
    fprintf(stderr, "at line %d:%d\n", line.number, line.pos);
    fprintf(stderr, " | %s\n", str_trim(line.start, line.end - line.start));
    fprintf(stderr, " %*s", line.pos, " ");
    fprintf(stderr, "^");
    for (int i = 0; i < token->length - 1; i++) {
        fprintf(stderr, "~");
    }
    fprintf(stderr, "\n%*s", line.pos-1, " ");
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    exit(1);
}
