#include "string_util.h"

#include <stdlib.h>
#include <string.h>

char* str_trim(char* str, int len) {
    char* result = calloc(len + 1, sizeof(char));
    memcpy(result, str, len);
    result[len] = '\0';
    return result;
}