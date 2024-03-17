#include <stdbool.h>
typedef enum {
    type_int,
    type_ptr
} typename_type;

typedef struct Typename Typename;
typedef struct Type_Array_info Type_Array_info;
struct Type_Array_info {
    int array_size;
};
struct Typename{
    typename_type type;
    Typename* ptr_to;
    Typename* point_this_type;
    Type_Array_info* array;
};

typedef struct {
    Typename* int_type;
} well_known_type;


int calc_var_size(Typename* type);
int calc_var_redister_size(Typename* type);
Typename* get_union_type();
bool is_type_conflict(Typename*,Typename*);
Typename* create_ptr_to(Typename*);
Typename* create_array_ptr(Typename*);
bool is_array_ptr(Typename*);
Typename* refer_ptr(Typename*);
well_known_type* get_wellknown_type();