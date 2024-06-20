#include "../Node/Node.h"

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
            return consume_typename(dictionary_get(defiened_struct_type, name->string, name->length));
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
        error_at(input, now_token->string, "excepted number");
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
