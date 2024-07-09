#ifndef UTIL_DICTIONARY
#define UTIL_DICTIONARY
#include <stdbool.h>
// Map(by Binary Tree)

typedef struct dictionary_struct dictionary_t;
struct dictionary_struct {
    dictionary_t* small;
    dictionary_t* big;
    char* key;
    int len;
    void* data;
};

void* dictionary_get(dictionary_t* dict, char* key, int len);
bool  dictionary_exist(dictionary_t* dict, char* text, int len);
void  dictionary_add(dictionary_t* dict, char* key, int len, void* data);



#endif