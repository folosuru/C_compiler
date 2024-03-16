#include "array_util.h"
#include <stdlib.h>
#include <stdio.h>
List_iter* add_reverse_array(List_iter* to) {
    List_iter* result = calloc(1, sizeof(List_iter));
    result->prev = to;
    if (to) {
        to->next = result;
        result->index = to->index;
    } else {
        result->index = calloc(1, sizeof(List_index));
        result->index->start = result;
    }
    result->index->end = result;
    return result;
}
List_iter* add_reverse_array_upd(List_iter** to) {
    *to = add_reverse_array(*to);
    return *to;
}
List_iter* insert_reverse_array(inseryt_type position, List_iter* insert_to) {
    if (!insert_to) {
        return add_reverse_array(0);
    }
    List_iter* result = calloc(1, sizeof(List_iter));
    result->index = insert_to->index;
    if (position == after_of) {
        if (insert_to->next) {
            result->next = insert_to->next;
            result->next->prev = result;
        } else {
            result->index->end = result;
        }
        result->prev = insert_to;
        insert_to->next = result;
    } else { // before
        if (insert_to->prev) {
            result->prev = insert_to->prev;
            result->prev->next = result;
        } else {
            result->index->start = result;
        }
        result->next = insert_to;
        insert_to->prev = result;
    }
    return result;
}