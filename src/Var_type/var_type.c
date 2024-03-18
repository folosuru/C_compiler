#include "var_type.h"
#include <stdlib.h>
#include <stdio.h>

Typename* get_union_type(Typename* p1, Typename* p2) {
    if (p1->type == type_ptr) {
        return p1;
    }
    if (p2->type == type_ptr) {
        return p2;
    }
    Typename* result;
    result = calloc(1, sizeof(Typename));
    result->type = type_int;
    return result;
}
bool is_type_conflict(Typename* p1,Typename* p2) {
    if (p1->type == type_ptr && p2->type == type_ptr) {
        Typename* p1_cur = p1;
        Typename* p2_cur = p2;
        if (p1_cur->type != type_ptr || p2_cur->type != type_ptr) {
            return false;
        }
        for (;;) {
            if (p1_cur->type != p2_cur->type) { // int* + int**
                return true;   
            }
            if (p1_cur->type == type_ptr) {
                p1_cur = p1_cur->ptr_to;
                p2_cur = p2_cur->ptr_to;
            } else {
                return false;
            }
        }
    } 
    return false;
}

int calc_var_redister_size(Typename* type) {
    switch (type->type){
        case type_char: {
            return 1;
        }

        case type_int: {
            return 4;
        }

        case type_ptr: {
            return 8;
        }
    
        default:
            return 0;
            break;
    }
}
int calc_var_size(Typename* type) {
    switch (type->type){
        case type_char: {
            return 1;
        }
        case type_int: {
            return 4;
        }

        case type_ptr: {
            if (type->array != 0) {
                return (calc_var_size(type->ptr_to) * type->array->array_size);
            }
            return 8;
        }
    
        default:
            return 0;
            break;
    }
}
Typename* create_ptr_to(Typename* p) {
    if (p->point_this_type) {
        return p->point_this_type;
    }
    Typename* result = calloc(1, sizeof(Typename));
    result->ptr_to = p;
    result->type = type_ptr;
    p->point_this_type = result;
    return result;
}

bool is_array_ptr(Typename* p) {
    if (p->type == type_ptr && p->array != 0) {
        return true;
    }
    return false;
}

Typename* refer_ptr(Typename* p) {
    return p->ptr_to;
}
well_known_type* wellknown_var;

well_known_type* get_wellknown_type() {
    if (!wellknown_var) {
        wellknown_var = calloc(1, sizeof(well_known_type));
        wellknown_var->int_type = calloc(1, sizeof(Typename));
        wellknown_var->int_type->type = type_int;
        wellknown_var->char_type = calloc(1, sizeof(Typename));
        wellknown_var->char_type->type = type_char;

    }
    return wellknown_var;
}