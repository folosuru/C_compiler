#include "../compiler.h"
#define expect_operator(name)\
     if (!consume_operator(name)) {\
        error_token(now_token, "%s %s", name, " not found!");\
    }

void error(char* fmt, ...) ;
void error_at(char* source, char* point, char* fmt, ...);
void error_token(Token* token, char* msg,  ...) ;
