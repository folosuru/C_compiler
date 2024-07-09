#include "var_type.h"
#include <stdlib.h>
#include <stdio.h>

dictionary_t* defiened_struct_type;

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
            if (p1_cur->type == type_void || p2_cur->type == type_void) {
                return false;
            }
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
        case type_void: {
            return 1;
        }
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
        case type_void: {
            return 1;
        }
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

        case type_struct: {
            return type->struct_data->size;
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
        wellknown_var->char_type = calloc(1, sizeof(Typename));
        wellknown_var->char_type->type = type_void;
    }
    return wellknown_var;
}


int calc_var_align_mlutiple(Typename* type) {
    return calc_var_size(type);
}

struct struct_define* create_unnamed_struct() {
    struct struct_define* result = calloc(1, sizeof(struct struct_define));
    result->member = calloc(1, sizeof(dictionary_t));
    result->var_type = calloc(1, sizeof(Typename));
    result->var_type->type = type_struct;
    result->var_type->struct_data = result;

    return result;
}

struct struct_define* create_struct_define(char* name, int name_len) {
    struct struct_define* result = calloc(1, sizeof(struct struct_define));
    result->name = name;
    result->name_len = name_len;
    result->member = calloc(1, sizeof(dictionary_t));

    Typename* type = calloc(1, sizeof(Typename));
    type->type = type_struct;
    type->struct_data = result;
    if (defiened_struct_type == 0) {
        defiened_struct_type = calloc(1, sizeof(dictionary_t));
    }
    result->var_type = type;
    dictionary_add(defiened_struct_type, name, name_len, type);
    return result;
}

struct struct_member* add_struct_member(struct struct_define* def, char* name, int name_len, Typename* type) {
    struct struct_member* member = calloc(1, sizeof(struct struct_member));
    member->name = name;
    member->name_len = name_len;
    member->type = type;
    if (dictionary_exist(def->member, name, name_len)) {
        
    }
    dictionary_add(def->member, name, name_len, member);
    int align_val;
    if (align_val = def->size % calc_var_align_mlutiple(type) != 0) {
        def->size += calc_var_align_mlutiple(type) - align_val;
    }
    member->offset = def->size;
    def->size += calc_var_size(type);
    return member;
}