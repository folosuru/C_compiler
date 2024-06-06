#include "dictionary.h"
#include <stdlib.h>
#include <string.h>

// left bigger than right
bool is_bigger_text(char* text, int len, char* text2, int len2) {
    if (text[0] >= text2[0]) { // Chu! 雑でごめん
        return true;
    }
    return false;
}

void  dictionary_add(dictionary_t* dict, char* key, int len, void* data) {
    dictionary_t* current = dict;
    if (dict->key == 0 && dict->len == 0) {
        dict->key = key;
        dict->len = len;
        dict->data = data;
        return;
    }
    dictionary_t* result = 0;
    while (true) {
        if (is_bigger_text( key, len, current->key, current->len)) {
            if (current->big == 0) {
                result = calloc(1, sizeof(dictionary_t));
                current->big = result;
                break;
            }
            current = current->big;
        } else {
            if (current->small == 0) {
                result = calloc(1, sizeof(dictionary_t));
                current->small = result;
                break;
            }
            current = current->small;
        }
    }
    result->key = key;
    result->len = len;
    result->data = data;
}

void* dictionary_get(dictionary_t* dict, char* key, int len) {
    dictionary_t* current = dict;
    while (true) {
        if (len == current->len) {
            if (memcmp(key, current->key, len) == 0) {
                return current->data;
            }
        }
        if (is_bigger_text( key, len, current->key, current->len)) {
            if (current->big == 0) {
                return 0;
            }
            current = current->big;
        } else {
            if (current->small == 0) {
                return 0;
            }
            current = current->small;
        }
    }
    return 0;
}

bool dictionary_exist(dictionary_t* dict, char* key, int len) {
    if (dictionary_get(dict, key, len) == 0) {
        return false;
    }
    return true;
}