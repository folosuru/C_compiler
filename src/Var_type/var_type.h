#ifndef VAR_TYPE_H
#define VAR_TYPE_H
#include <stdbool.h>
#include "../util/dictionary.h"
typedef enum {
    type_int,
    type_char,
    type_void,
    type_ptr,
    type_struct
} typename_type;

typedef struct Typename Typename;
typedef struct Type_Array_info Type_Array_info;
struct Type_Array_info {
    int array_size;
};
struct struct_define {
    char* name;
    int name_len;
    dictionary_t* member;
    int size;
};
struct Typename{
    typename_type type;
    Typename* ptr_to;
    Typename* point_this_type;
    Type_Array_info* array;
    struct struct_define* struct_data;
};

typedef struct {
    Typename* int_type;
    Typename* char_type;
    Typename* void_type;
} well_known_type;


int calc_var_size(Typename* type);
int calc_var_redister_size(Typename* type);
int calc_var_align_mlutiple(Typename* type);
Typename* get_union_type();
bool is_type_conflict(Typename*,Typename*);
Typename* create_ptr_to(Typename*);
Typename* create_array_ptr(Typename*);
bool is_array_ptr(Typename*);
Typename* refer_ptr(Typename*);
well_known_type* get_wellknown_type();

extern dictionary_t* defiened_struct_type;

struct struct_member {
    int offset;
    Typename* type;
    char* name;
    int name_len;
};


struct struct_define* create_struct_define(char* name, int name_len);
void add_struct_member(struct struct_define* def, char* name, int name_len, Typename* type);

#endif