#include "Node.h"
#include "../Var_type/var_type.h"

void struct_define_node() {
    Token* name = consume_identify();
    if (!name) {
        error_token(now_token, "need struct name");
    }
    if (!consume_operator("{")) {
        error_token(now_token, "need \"{\"");
    }
    struct struct_define* result = create_struct_define(name->string, name->length); 
    Typename* type;
    while (type = consume_typename(0)) {
        Token* name = consume_identify();
        if (!name) {
            error_token(now_token, "need field name");
        }
        add_struct_member(result, name->string, name->length, type);
        if (!consume_operator(";")) {
            error_token(now_token, "need ;");
        }
    }
    consume_operator("}");
    consume_operator(";");
    
}