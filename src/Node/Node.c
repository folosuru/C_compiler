#include "Node.h"
#include "../util/array_util.h"
#include "../error/print_error.h"
#include "../util/string_util.h"
Local_var* createLocalVarOffsaet(char* name, int length, Typename* type);


bool at_eof() {
  return now_token->type == TOKEN_EOF;
}

Node* new_node_num(int val);
Node* new_node(NodeType type, Node* left, Node* right) {
    Node *node = calloc(1, sizeof(Node));
    if (is_type_conflict(left->var_type, right->var_type)) {
        error("type duplicate!");
    }
    node->var_type = get_union_type(left->var_type, right->var_type);
    if (node->var_type->type == type_ptr) {
        if (left->var_type->type != type_ptr) {
            node->left = type_convention(new_node(NODE_MLU, new_node_num(calc_var_size(node->var_type)), left), node->var_type);
            node->right = right;
        }
        if (right->var_type->type != type_ptr) {
            node->left = left;
            node->right = type_convention(new_node(NODE_MLU, new_node_num(calc_var_size(node->var_type)), right), node->var_type);
        }
        if (right->var_type->type == left->var_type->type && right->var_type->type == type_ptr) {
            node->left = left;
            node->right = right;
        }
    } else {
        node->left = left;
        node->right = right;
    }
    node->type = type;
  return node;
}
Node* new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->type = NODE_NUM;
  node->value = val;
  node->var_type = calloc(1, sizeof(Typename));
  node->var_type->type = type_int;
  return node;
}
Node* get_NodeTree() {
    Node* result = statement();
    if (!at_eof()) {
        error_at(input, now_token->string, "unknown token");
    }
    return result;
}

function_def* working_function;
void Lvar_offset_calc(List_index* index);
// function_def*
List_iter* found_function;
int calc_arg_offset();

Args_var* createArgsVarOffsaet(char* name, int length, Typename* type, int index) {
    Args_var* old_var = 0;
    if (working_function->args_list) {
        old_var = working_function->args_list->data;
    }
    List_iter* result = add_reverse_array_upd(&working_function->args_list);
    Args_var* new_var = calloc(1,sizeof(Args_var));
    if (old_var) {
        new_var->offset = old_var->offset + old_var->size;
    } else {
        new_var->offset = 16;
    }
    new_var->name = name;
    new_var->len = length;
    new_var->size = 8;
    new_var->index = index;
    new_var->type = type;
    result->data = new_var;
    return new_var;
}

function_def* getFunction() {
    if (at_eof()) {
        return 0;
    }
    Typename* type;
    type = consume_typename(0);
    if (!type) {
        error_token(now_token, "nanikore?");
    }
    working_function = calloc(4, sizeof(function_def));
    working_function->return_value = type;
    Token* name = consume_identify();
    if (!name) {
        error_token(now_token, "need function name");
    }
    working_function->name = name->string;
    working_function->name_length = name->length;
    consume_operator("(");
    int args_cnt = 0;
    while (!consume_operator(")")) {
        Typename* arg_type = consume_typename(0);
        if (!arg_type) {
            error_token(now_token, "need typename");
        }
        Token* arg_name = consume_identify();
        consume_operator(",");
        for (List_iter* current = working_function->args_list; current != 0; current = current->prev ) {
            Args_var* current_var = current->data;
            if (current_var->len == arg_name->length && !memcmp(current_var->name, arg_name->string, current_var->len)) {
                error_token(arg_name, "Arg name Duplicate");
            }
        }
        working_function->args_count++;
        createArgsVarOffsaet(arg_name->string, arg_name->length, arg_type, working_function->args_count);
    }
    consume_operator("{");
    working_function->program = getProgram();
    if (working_function->lvar) {
        Lvar_offset_calc(working_function->lvar->index);
    }
    add_reverse_array_upd(&found_function)->data = working_function;
    return working_function;
}

Program* getProgram() {
    Program head;
    Program* current = &head;
    int statement_count = 0;
    while (!consume_operator("}") /*&& !at_eof()*/) {
        statement_count++;
        Program* next_statement = calloc(1, sizeof(Program));
        next_statement->node = statement();
        next_statement->next = 0;
        current->next = next_statement;
        current = next_statement;
    }
    if (statement_count == 0) {
        return 0;
    }
    return head.next;
}

Node* statement() {
    Node* node;
    if (consume_preserved(KEYWORD_IF)) {
        node = calloc(1, sizeof(Node));
        node->data = calloc(1, sizeof(IF_Node_data));
        IF_Node_data* data = node->data;
        node->type = NODE_IF;
        data->condition = assign();
        data->statement = statement();
        if (consume_preserved(KEYWORD_ELSE)) {
            data->else_statement = statement();
        }
        return node;
    } else if (consume_preserved(KEYWORD_FOR)) {
        node = calloc(1, sizeof(Node));
        node->type = NODE_FOR;
        node->data = calloc(1, sizeof(FOR_Node_data));
        FOR_Node_data* data = node->data;
        if (!consume_operator("(")) {
            error_token(now_token->next, "need (");
        }
        if (!consume_operator(";")) {
            data->init = assign_or_declaration();
            consume_operator(";");
        }
        if (!consume_operator(";")) {
            data->condition = assign();
            consume_operator(";");
        }
        if (!consume_operator(")")) {
            data->iterate = assign();
            consume_operator(")");
        }
        data->code = statement();
        return node;
    } else if (consume_operator("{")) {
        node = calloc(1, sizeof(Node));
        node->type = NODE_BLOCK;
        node->data = getProgram();
        return node;
    } else {
        Typename* type;
        if (consume_preserved(KEYWORD_RETURN)) {
            node = calloc(1, sizeof(Node));
            node->type = NODE_RETURN;
            node->left = assign();
        } else if (type = consume_typename(0)) {
            Token* name = consume_identify();
            if (!name) {
                error_token(now_token, "need variable name");
            }
            createLocalVarOffsaet(name->string, name->length, type);
            if (consume_operator("=")) {
                now_token = name;
                node = assign();
            } else {
                node = calloc(1, sizeof(Node));
                node->type = NODE_DO_NOTHING;
            }
        } else {
            node = assign();
        }


        if (consume_operator(";")) {
            return node;
        } else {
            error_at(input, now_token->string, "missing ; ");
        }
    }
}

Node* assign_or_declaration() {
    Node* node;
    Typename* type;
    if (type = consume_typename(0)) {
        Token* name = consume_identify();
        if (!name) {
            error_token(now_token, "need variable name");
        }
        createLocalVarOffsaet(name->string, name->length, type);
        if (consume_operator("=")) {
            now_token = name;
        } else {
            node = calloc(1, sizeof(Node));
            node->type = NODE_DO_NOTHING;
            return node;
        }
    }
    node = assign();
    return node;
}

Node* assign() {
    Node* node = equal_operator();
        while (true) {
        if (consume_operator("=")) {
            return new_node(NODE_ASSIGN, node, assign());
        } else {
            return node;
        }
    }
}
Node* equal_operator() {
    Node* node = rerational_operator();  // 左辺
    while (true) {
        if (consume_operator("==")) {
            node = new_node(NODE_EQUAL, node, rerational_operator()/* ここで右辺 を求    */ );
        } else if (consume_operator("!=")) {
            node = new_node(NODE_NOT_EQUAL, node, rerational_operator());
        } else {
            return node;
        }
    }

}
Node* rerational_operator() {
    Node* node = add_sub_operator();  // 左辺
    while (true) {
        if (consume_operator("<")) {
            node = new_node(NODE_SMALLER, node, add_sub_operator()/* ここで右辺 を求  */ );
        } else if (consume_operator(">")) {
            node = new_node(NODE_SMALLER, add_sub_operator(), node );
        } else if (consume_operator("<=")) {
            node = new_node(NODE_SMALLER_OR_EQUAL, node, add_sub_operator());
        } else if (consume_operator(">=")) {
            node = new_node(NODE_SMALLER_OR_EQUAL,add_sub_operator(), node);
        } else {
            return node;
        }
    }
}
Node* add_sub_operator() {
    Node* node = mlu();  // 左辺
    while (true) {
        if (consume_operator("+")) {
            node = new_node(NODE_ADD, node, mlu()/* ここで右辺 を求  */ );
        } else if (consume_operator("-")) {
            node = new_node(NODE_SUB, node, mlu());
        } else {
            return node;
        }
    }
}

// contain * / & primary
Node* mlu() {
    Node* node = unary();  // 左辺
    while (true) {
        if (consume_operator("*")) {
            node = new_node(NODE_MLU, node, unary()/* ここで右辺 を求  */ );
        } else if (consume_operator("/")) {
            node = new_node(NODE_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node* unary() {
    while (true) {
        Token* current_token = now_token;
        if (consume_operator("+")) {
            return primary();
        } else if (consume_operator("-")) {
            return new_node(NODE_SUB, new_node_num(0), primary());
        } else if (consume_operator("*")) {
            Node* ref_to = unary();
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_REFER;
            node->left = ref_to;
            if (ref_to->var_type->type != type_ptr) {
                error_token(current_token, "after refer operator need Pointer type");
            }
            node->var_type = refer_ptr(ref_to->var_type);
            return node;
        } else if (consume_operator("&")) {
            Node* ref_to = unary();
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_DEREFER;
            node->left = ref_to;
            node->var_type = create_ptr_to(ref_to->var_type);
            return node;
        } else if (consume_preserved(KEYWORD_SIZEOF)) {
            bool in_bracket = consume_operator("(");
            Typename* type = consume_typename(0);
            int return_var;
            if (type) {
                return_var = calc_var_size(type);
            } else {
                return_var = calc_var_size(unary()->var_type);
            }
            if (in_bracket) {
                consume_operator(")");
            }
            return new_node_num(return_var);
            
        } else {
            return primary();
        }
    }
}

Local_var* createLocalVarOffsaet(char* name, int length, Typename* type) {
    List_iter* result;
    if (working_function->lvar) {
        List_iter* iter_cur = working_function->lvar->index->start;
        while (true) {
            if (iter_cur) {
                Local_var* cur_var = iter_cur->data;
                if (cur_var->size <= calc_var_size(type)) {
                    result = insert_reverse_array(before_of ,iter_cur);
                    printf("# !! %s\n", str_trim(name, length));
                    break;
                }
                iter_cur = iter_cur->next;
            } else {
                result = add_reverse_array_upd(&working_function->lvar);
                break;
            }
        }
    } else {
        result = add_reverse_array_upd(&working_function->lvar);
    }
    result->data = calloc(1,sizeof(Local_var));
    Local_var* new_var = result->data;
    working_function->total_offset = working_function->total_offset + calc_var_size(type);
    new_var->name = name;
    new_var->len = length;
    new_var->size = calc_var_size(type);
    new_var->type = type;
    return new_var;
}

Local_var* getLocalVarOffset(Token* token) {
    for (List_iter* current = working_function->lvar; current != 0; current = current->prev ) {
        Local_var* current_var = current->data;
        if (current_var->len == token->length) {
            if (memcmp(current_var->name, token->string, current_var->len) == 0) {
                return current_var;
            }
        }
    }
    return 0;
}

Args_var* getArgsVarOffset(Token* token) {
    for (List_iter* current = working_function->args_list; current != 0; current = current->prev ) {
        Args_var* current_var = current->data;
        if (current_var->len == token->length) {
            if (memcmp(current_var->name, token->string, current_var->len) == 0) {
                return current_var;
            }
        }
    }
    return 0;
}

function_def* find_function(Token* name) {
    for (List_iter* current = found_function; current != 0; current = current->prev ) {
        function_def* current_var = current->data;
        if (current_var->name_length != name->length) continue;
        if (memcmp(current_var->name, name->string, current_var->name_length) == 0) {
            return current_var;
        }
    }

    return 0;
}

void function_call_check(Token* function_name, function_def* call_func, int args_count) {
    if (call_func->args_count > args_count) {
        error_token(function_name, "function %s need %d args, but only %d",
                    str_trim(call_func->name, call_func->name_length),
                    call_func->args_count, 
                    args_count);
    }

    return;
}

Node* primary() {
    if (consume_operator("(")) {
        Node *node = assign();
        expect_operator(")");
        return node;
    }
    Token* variable = consume_identify();
    if (variable == 0) {
        return new_node_num(except_number());
    }
    if(consume_operator("(")) {  // function call
        Node* node = calloc(1, sizeof(Node));
        node->type = NODE_FUNC_CALL;
        FUNC_CALL_NODE_Data* data = calloc(1, sizeof(FUNC_CALL_NODE_Data));
        node->data = data;
        data->name = variable->string;
        data->len = variable->length;
        List_iter* args_list = 0;
        while (!consume_operator(")")) {
            args_list = add_reverse_array(args_list);
            args_list->data = assign();
            data->args_cnt++;
            consume_operator(",");
        }
        data->args = args_list;

        function_def* call_function = find_function(variable);
        if (!call_function) {
            node->var_type = get_wellknown_type()->int_type;    
        } else {
            node->var_type = call_function->return_value;
            function_call_check(variable, call_function, data->args_cnt);
        }
        return node;
    } else {
        Node* node = calloc(1, sizeof(Node));

        Local_var* lvar = getLocalVarOffset(variable);
        if (lvar) {
            node->type = NODE_LOCALVALUE;
            node->data = lvar;
            node->var_type = lvar->type;
            node->data = lvar;
            return node;
        }

        Args_var* args_var = getArgsVarOffset(variable);
        if (args_var) {
            node->type = NODE_ARG;
            node->data = args_var;
            node->var_type = args_var->type;
            return node;
        }

        if (!lvar) {
            error_token(variable, "unknown identify");
        }
    }
}


int calc_arg_offset(int arg_index) {
    #ifdef ABI_LINUX
    #else 
    return -16 - (8 * arg_index);
    #endif
}

int align_8(int v) {
    if (v % 8 == 0) {
        return v;
    }
    return ((v / 8) + 1) * 8;
}

int calc_total_offset() {
    #ifdef ABI_LINUX
    return total_offset;
    #else

    return align_8(working_function->total_offset + 8);
    #endif
}

Node* type_convention(Node* node, Typename* cast_to) {
    Node* result = calloc(1, sizeof(Node));
    result->left = node;
    result->type = NODE_TYPE_CONVENTION;
    result->var_type = cast_to;
    return result;
}

void Lvar_offset_calc(List_index* index) {
    List_iter* cur = index->start;
    while (cur) {
        Local_var* new_var = cur->data;
        if (cur->prev) {
            Local_var* old_var = cur->prev->data;
            new_var->offset = old_var->offset + old_var->size;
        } else {
            new_var->offset = 8;
        }
        cur = cur->next;
    }
    
}