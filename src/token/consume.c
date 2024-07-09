#include "../Node/Node.h"
#include "../util/string_util.h"
Token* consume_identify() {
    if (now_token->type == TOKEN_IDENTIFY) {
        Token* result = now_token;
        now_token = now_token->next;
        return result;
    }
    return 0;
}

bool consume_preserved(Preserved_Word word) {
    if (now_token->type == TOKEN_PRESERVED_WORD && now_token->value == word) {
        now_token = now_token->next;
        return true;
    }
    return false;
}


bool consume_operator(char* operator) {
    if (strlen(operator) != now_token->length) {
        return false;
    }
    if (now_token->type == TOKEN_PREFIX && memcmp(now_token->string, operator, now_token->length) == 0) {
        now_token = now_token->next;
        return true;
    } else {
        return false;
    }
}

Typename* create_typename(typename_type t, Typename* ptr_to) {
    if (t == type_int) {
        return get_wellknown_type()->int_type;
    }
    
    Typename* result = calloc(1, sizeof(Typename));
    result->type = t;
    result->ptr_to = ptr_to;
    return result;
}


Typename* consume_typename(Typename* parent) {

    if (now_token->type == TOKEN_PRESERVED_WORD ) { 
        if ( now_token->value == KEYWORD_INT) {
            now_token = now_token->next;
            return consume_typename( create_typename(type_int,0) );
        } else if (now_token->value == KEYWORD_CHAR) {
            now_token = now_token->next;
            return consume_typename( create_typename(type_char,0) );
        } else if (now_token->value == KEYWORD_VOID) {
            now_token = now_token->next;
            return consume_typename( create_typename(type_void,0) );
        } else if (consume_preserved(KEYWORD_STRUCT)) {
            Token* name = consume_identify();
            
            struct struct_define* struct_def;

            if (name) {
                if (defiened_struct_type == 0) {
                    defiened_struct_type = calloc(1, sizeof(dictionary_t));
                }
                void* result = dictionary_get(defiened_struct_type, name->string, name->length);
                if (result != 0) {
                    return consume_typename(result);
                } else {  // named struct
                    struct_def = create_struct_define(name->string, name->length);
                    printf("# struct %s\n", str_trim(name->string, name->length));
                }
            } else { // unnamed struct
                struct_def = create_unnamed_struct();
                printf("# unnamed struct");
            }
            if (!consume_operator("{")) {
                error_token(now_token, "must be {");
            }

            Typename* type;
            while (type = consume_typename(0)) {
                Token* name = consume_identify();
                if (!name) {
                    error_token(now_token, "need field name");
                }
                struct struct_member* member = add_struct_member(struct_def, name->string, name->length, type);
                if (!consume_operator(";")) {
                    error_token(now_token, "need ;");
                }
//                printf("#  %7s | offset: %d\n", str_trim(name->string, name->length), member->offset);
            }
            consume_operator("}");
            return struct_def->var_type;
        }
    }
    if (parent == 0) {
        return 0;
    }
    if (consume_operator("*")) {
        return consume_typename( create_typename(type_ptr,parent) );
    }
    return parent;

}

int except_number() {
    if (now_token->type != TOKEN_NUM) {
        error_at(input, now_token->string, "excepted number, function call or expression");
    }
    int result = now_token->value;
    now_token = now_token->next;
    return result;
}


string_literal_data* consume_text_literal() {
    if (now_token->type == TOKEN_STRING) {
        string_literal_data* literal_id = now_token->data.str_data;
        now_token = now_token->next;
        return literal_id;
    } else {
        return 0;
    }
}
