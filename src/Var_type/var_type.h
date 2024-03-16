#include <stdbool.h>
typedef enum {
    type_int,
    type_ptr,
    type_array
} typename_type;

typedef struct Typename Typename;
struct Typename{
    typename_type type;
    Typename* ptr_to;
    Typename* point_this_type;
};

typedef struct {
    Typename* int_type;
} well_known_type;


int calc_var_size(Typename* type);
Typename* get_union_type();
bool is_type_conflict(Typename*,Typename*);
Typename* create_ptr_to(Typename*);
Typename* refer_ptr(Typename*);
well_known_type* get_wellknown_type();