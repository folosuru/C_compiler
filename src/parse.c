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
        string[0] == '[' || string[0] == ']' ) {
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

Token* tokenize(char* p) {
    Token first;
    Token* current = &first;
    char* now = p;
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

        error_at(p, now, "unexpected token");
    }
    create_token(TOKEN_EOF, current,  p +strlen(p),0);
    return first.next;
}
