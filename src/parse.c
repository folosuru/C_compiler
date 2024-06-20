#include "compiler.h"
#include "Node/Node.h"
Token *now_token;    
bool is_variable_char(char value) {
    return (('a' <= value && value <= 'z') ||
            ('A' <= value && value <= 'Z') ||
            ('0' <= value && value <= '9') ||
            value == '_');
}


Token* create_token(TokenType type, Token* join_to, char* string, int length) {
    Token *tok = calloc(1, sizeof(Token));
    tok->type = type;
    tok->string = string;
    tok->length = length;
    join_to->next = tok;
    return tok; 
}
int is_operator(char* string) {
    if (strncmp(string, ">=", 2) == 0 ||
        strncmp(string, "<=", 2) == 0 ||
        strncmp(string, "==", 2) == 0 ||
        strncmp(string, "!=", 2) == 0 
        ) {
        return 2;
    }
    if (string[0] == '>' || string[0] == '<' ||
        string[0] == '+' || string[0] == '-' || string[0] == '*' || string[0] == '/' || 
        string[0] == '(' || string[0] == ')' || string[0] == ';' || string[0] == '=' ||
        string[0] == '{' || string[0] == '}' || string[0] == ',' || string[0] == '&' ||
        string[0] == '[' || string[0] == ']' || string[0] == '.') {
        return 1;
    }
    return 0;
}

bool keyword_match(char* str, char* cmp) {
    return (strncmp(str, cmp, strlen(cmp)) == 0 && !is_variable_char(*(str+strlen(cmp)))) ;
}

int is_preserved_keyword(char* str, Preserved_Word* word) {
    if (keyword_match(str, "return")) {
        *word = KEYWORD_RETURN;
        return 6;
    }
    if (keyword_match(str, "if")) {
        *word = KEYWORD_IF;
        return 2;
    }
    if (keyword_match(str, "else")) {
        *word = KEYWORD_ELSE;
        return 4;
    }
    if (keyword_match(str, "for")) {
        *word = KEYWORD_FOR;
        return 3;
    }
    if (keyword_match(str, "while")) {
        *word = KEYWORD_WHILE;
        return 5;
    }
    if (keyword_match(str, "int")) {
        *word = KEYWORD_INT;
        return 3;
    }
    if (keyword_match(str, "sizeof")){
        *word = KEYWORD_SIZEOF;
        return 6;
    }
    if (keyword_match(str, "char")){
        *word = KEYWORD_CHAR;
        return 4;
    }
    if (keyword_match(str, "void")){
        *word = KEYWORD_VOID;
        return 4;
    }
    if (keyword_match(str, "struct")){
        *word = KEYWORD_STRUCT;
        return 6;
    }
    
    return 0;
}


int get_variable_token(char* now_ptr) {
    int var_length = 0;
    while (*now_ptr) {
        if (is_variable_char(*now_ptr)) {
            var_length++;
            now_ptr++;
        } else {
            break;
        }
    }
    return var_length;
}

struct string_literal_char_info {
    int source_length;
    int string_length;
};

struct string_literal_char_info get_string_literal_1char_length(char* string) {
    struct string_literal_char_info result;
    result.source_length = 1;
    result.string_length = 1;
    if (*string == '\0' || *string == '"') {
        result.source_length = 0;
        result.string_length = 0;
    }
    if (*string == '\\') {
        result.source_length = 2;
    }
    return result;
}

int get_string_literal_length(char** current) {
    int result = 0;
    while (true) {
        struct string_literal_char_info len_data = get_string_literal_1char_length(*current);
        if (len_data.source_length == 0) {
            break;
        }
        *current = *current + len_data.source_length;
        result += len_data.string_length;
    }
    return result + 1; // \0
}

char* create_string_literal_text(char** current_char) {
    char* scanner = *current_char;
    int text_count = 0;
    scanner++;
    while (true) {
        if (*scanner == '"') {
            break;
        }
        text_count++;
    }
    (*current_char)++;
    char* result = calloc(text_count+1, sizeof(char));
    int scan_pos = 0;
    for (int text_count_scan = 0; text_count_scan > text_count; ) {
        result[text_count_scan] = **current_char;
        (*current_char)++;
    }
    (*current_char)++;
    return result;
}


struct tokenize_result* tokenize(char* p) {
    struct tokenize_result* result = calloc(1, sizeof(struct tokenize_result));
    Token first;
    Token* current = &first;
    char* now = p;
    int text_literal_id = 1;
    while (*now) {
        if (isspace(*now)) {
            now++;
            continue;
        }
        if (isdigit(*now)) {
            current = create_token(TOKEN_NUM, current, now, 0);
            current->value = strtol(now, &now, 10);
            continue;
        }
        int operator_length = is_operator(now);
        if (operator_length != 0) {
            current = create_token(TOKEN_PREFIX, current, now, operator_length);
            now = now + operator_length;
            continue;
        }
        
        Preserved_Word word;
        int keyword_length = is_preserved_keyword(now, &word);
        if (keyword_length != 0) {
            current = create_token(TOKEN_PRESERVED_WORD, current, now, keyword_length);
            current->value = word;
            now = now + keyword_length;
            continue;
        }
        if (is_variable_char(*now)) {  // 複数文字　やるぞ～～～
            int var_length = get_variable_token(now);
            current = create_token(TOKEN_IDENTIFY, current, now, var_length);
            now = now + var_length;
            continue;
        }
        if (*now == '"') {
            char* str_start_pos = now;
            now++;
            if (current->type == TOKEN_STRING) {}
            int literal_source_len = get_string_literal_length(&now);
            List_iter* text_literal_list =  add_reverse_array_upd(&result->string_literal);
            string_literal_data* text_literal_data = calloc(1, sizeof(string_literal_data));
            text_literal_list->data = text_literal_data;
            text_literal_data->text = str_start_pos;
            text_literal_data->literal_length = (now - str_start_pos) / sizeof(char);
            text_literal_data->string_length = literal_source_len;
            text_literal_data->id = text_literal_id;

            current = create_token(TOKEN_STRING, current, str_start_pos, (now - str_start_pos) / sizeof(char));
            current->value = text_literal_id;
            current->data.str_data = text_literal_data;

            text_literal_id++;
            now++;
            continue;
        }

        error_at(p, now, "unexpected token");
    }
    create_token(TOKEN_EOF, current,  p +strlen(p),0);
    result->token = first.next;
    return result;
}
