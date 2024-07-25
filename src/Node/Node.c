#include "Node.h"
#include "../util/array_util.h"
#include "../error/print_error.h"
#include "../util/string_util.h"
#include "../util/dictionary.h"
Local_var* createLocalVarOffsaet(char* name, int length, Typename* type);


bool at_eof() {
  return now_token->type == TOKEN_EOF;
}

Node* new_node_num(int val);
Node* new_node_plain(NodeType type, Node* left, Node* right, Typename* var_type) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    node->left = left;
    node->right = right;
    node->var_type = var_type;
    return node;
}

Node* new_node(NodeType type, Node* left, Node* right) {
    Node *node = calloc(1, sizeof(Node));
    if (is_type_conflict(left->var_type, right->var_type)) {
        error("type duplicate!");
    }
    node->var_type = get_union_type(left->var_type, right->var_type);
    if (node->var_type->type == type_ptr) {
        if (left->var_type->type != type_ptr) {
            node->left = type_convention(new_node(NODE_MLU, new_node_num(calc_var_size(refer_ptr(node->var_type))), left), node->var_type);
            node->right = right;
        }
        if (right->var_type->type != type_ptr) {
            node->left = left;
            node->right = type_convention(new_node(NODE_MLU, new_node_num(calc_var_size(refer_ptr(node->var_type))), right), node->var_type);
        }
        if (right->var_type->type == type_ptr) {
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

Node* createStatementNode(Node* stm) {
    Node *node = calloc(1, sizeof(Node));
    node->type = NODE_STATEMENT;
    node->left = stm;
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
dictionary_t found_function;
dictionary_t found_global_var;
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

asm_label_def* getFunction() {
    if (at_eof()) {
        return 0;
    }
    Typename* type;

    type = consume_typename(0);
    if (!type) {
        error_token(now_token, "nanikore?");
    }
    Token* name = consume_identify();
    if (!name) {
        consume_operator(";");
        asm_label_def* result = calloc(1, sizeof(asm_label_def));
        result->nothing = true;
        return result;
    }

    if (consume_operator("(")) {
        working_function = calloc(4, sizeof(function_def));
        working_function->return_value = type;
        working_function->name = name->string;
        working_function->name_length = name->length;
        int args_cnt = 0;
        bool arg_empty_flag = false;
        Token* empty_arg = 0;
        while (!consume_operator(")")) {
            Typename* arg_type = consume_typename(0);
            if (!arg_type) {
                error_token(now_token, "need typename");
            }
            Token* arg_name = consume_identify();
            if (!arg_name) {
                arg_empty_flag = true;
                if (!empty_arg) {
                    empty_arg = now_token;
                }
                createArgsVarOffsaet(0, 0, arg_type, working_function->args_count);
            } else {
                for (List_iter* current = working_function->args_list; current != 0; current = current->prev ) {
                    Args_var* current_var = current->data;
                    if (current_var->len == arg_name->length && !memcmp(current_var->name, arg_name->string, current_var->len)) {
                        error_token(arg_name, "Arg name Duplicate");
                    }
                }    
                createArgsVarOffsaet(arg_name->string, arg_name->length, arg_type, working_function->args_count);
            }
            working_function->args_count++;
            consume_operator(",");
        }
        if (consume_operator("{")) {
            if (arg_empty_flag) {
                error_token(empty_arg, "function impl needs arg name");
            }
            working_function->program = getProgram();
            if (working_function->lvar) {

                Lvar_offset_calc(working_function->lvar->index);
            }
        } else {
            consume_operator(";");
        }
        dictionary_add(&found_function, working_function->name, working_function->name_length, working_function);
        asm_label_def* result = calloc(1, sizeof(asm_label_def));
        result->func = working_function;
        return result;
    } else if (consume_operator(";")) {
        asm_label_def* result = calloc(1, sizeof(asm_label_def));
        result->variable = calloc(1, sizeof(Globalvar_def));
        result->variable->is_impl = true;
        result->variable->name = str_trim(name->string, name->length);
        result->variable->length = name->length;
        result->variable->type = type;
        dictionary_add(&found_global_var, result->variable->name, result->variable->length, result->variable);
        return result;
    }
    return 0;
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
        return if_node();
    } else if (consume_preserved(KEYWORD_FOR)) {
        return for_node();
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
            List_iter* array_list = 0;
            while (consume_operator("[")) {
                add_reverse_array_upd(&array_list);
                array_list->data = ((void*)except_number());
                consume_operator("]");
            }
            while (array_list) {
                Typename* new_type = calloc(1, sizeof(Typename));
                new_type->ptr_to = type;
                new_type->type = type_ptr;
                new_type->array = calloc(1, sizeof(Type_Array_info));
                new_type->array->array_size  = ((int)array_list->data);
                type = new_type;
                array_list = array_list->prev;
            }
            createLocalVarOffsaet(name->string, name->length, type);
            if (consume_operator("=")) {
                now_token = name;
                node = assign();
            } else {
                node = calloc(1, sizeof(Node));
                node->type = NODE_DO_NOTHING;
                if (consume_operator(";")) {
                    return node;
                } else {
                    error_token(now_token, "need ;");
                }
            }
        } else {
            node = assign();
        }


        if (consume_operator(";")) {
            return createStatementNode(node);
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
            return menber_access();
        } else if (consume_operator("-")) {
            return new_node(NODE_SUB, new_node_num(0), menber_access());
        } else if (consume_operator("*")) {
            Node* ref_to = unary();
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_REFER;
            node->left = ref_to;
            if (ref_to->var_type->type != type_ptr ) {
                error_token(current_token, "after refer operator need Pointer type");
            }
            node->var_type = refer_ptr(ref_to->var_type);
            return node;
        } else if (consume_operator("&")) {
            Node* ref_to = unary();
            if (is_array_ptr(ref_to->var_type)) {
                return ref_to;
            }
            if (ref_to->type == NODE_REFER) {
                ref_to->left->var_type = create_ptr_to(ref_to->left->var_type);
                return ref_to->left;
            }
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
            return menber_access();
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

Globalvar_def* getGlobalVar(Token* token) {
    return dictionary_get(&found_global_var, token->string, token->length);
}

function_def* find_function(Token* name) {
    return dictionary_get(&found_function, name->string, name->length);
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

Node* menber_access() {
    Token* current_token = now_token;
    Node* node  = primary();
    Node* result = node;
    // a[1][2] = (sizeof(*a) * 1) + (sizeof(**a) * 2) + &a; 
    // a[1][2] = *((sizeof(*a) * 1) + (sizeof(**a) * 2) + &a); 
    while (true) {
        Token* operator_token = now_token;
        if (consume_operator("[")) {  // 汚くてわけが分からない
            Node* result_offset = node;
            Typename* access_to_type = node->var_type;
            do {
                if (access_to_type->type != type_ptr) {
                    error_token(current_token, "this type cannnot access(not pointer, or array)");
                }
                access_to_type = access_to_type->ptr_to;
                Node* add_offset = new_node_plain(NODE_MLU, new_node_num(calc_var_size(access_to_type)), assign(), access_to_type);
                result_offset = new_node_plain(NODE_ADD, result_offset, add_offset, access_to_type);
            consume_operator("]");
            } while (consume_operator("["));
            result = new_node_plain(NODE_REFER, result_offset,0 , access_to_type);
        } else if (consume_operator(".")) {
            result = new_node_plain(NODE_DEREFER, result, 0, create_ptr_to(result->var_type));
            struct struct_member* member = 0;
            do {
                if (result->var_type->ptr_to->type != type_struct) {
                    error_token(operator_token, "value is not struct type");
                }
                Token* target = consume_identify();
                if (target == 0) {
                    //
                }
                struct struct_define* struct_def = result->var_type->ptr_to->struct_data;
                member = dictionary_get(struct_def->member, target->string, target->length);
                if (member == 0) {
                    error_token(target, "member not found");
                }
                result = new_node_plain(NODE_ADD, result , new_node_num(member->offset),  create_ptr_to(member->type));
            } while (consume_operator("."));
            result = new_node_plain(NODE_REFER, result, 0, member->type);
        } else if (consume_operator("->")) {
            struct struct_member* member = 0;
            do {
                if (result->var_type->ptr_to->type != type_struct) {
                    error_token(operator_token, "value is not struct type");
                }
                Token* target = consume_identify();
                if (target == 0) {
                    //
                }
                struct struct_define* struct_def = result->var_type->ptr_to->struct_data;
                member = dictionary_get(struct_def->member, target->string, target->length);
                if (member == 0) {
                    error_token(target, "member not found");
                }
                result = new_node_plain(NODE_ADD, result , new_node_num(member->offset), create_ptr_to(member->type));
            } while (consume_operator("."));
            result = new_node_plain(NODE_REFER, result, 0, member->type);
        } else {
            break;
        }
    }
    return result;
}


Node* primary() {
    if (consume_operator("(")) {
        Node *node = assign();
        expect_operator(")");
        return node;
    }
    string_literal_data* string_literal = consume_text_literal();
    if (string_literal != 0) {
        Node* node = calloc(1, sizeof(Node));
        node->type = NODE_STRING_LITERAL;
        node->value = string_literal->id;
        node->var_type = calloc(1, sizeof(Typename));

                node->var_type->ptr_to = get_wellknown_type()->char_type;
                node->var_type->type = type_ptr;
                node->var_type->array = calloc(1, sizeof(Type_Array_info));
                node->var_type->array->array_size  = (string_literal->string_length);
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
            if (is_array_ptr(node->var_type)) {
                Node* result = calloc(1, sizeof(Node));
                result->type = NODE_DEREFER;
                result->left = node;
                result->var_type = node->var_type;
                return result;
            }
            return node;
        }

        Args_var* args_var = getArgsVarOffset(variable);
        if (args_var) {
            node->type = NODE_ARG;
            node->data = args_var;
            node->var_type = args_var->type;
            return node;
        }

        Globalvar_def* global_bar = getGlobalVar(variable);
        if (global_bar) {
            node->type = NODE_GLOBAL_VAR;
            node->data = global_bar;
            node->var_type = global_bar->type;
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

int calc_array_first_offset(Typename* var) {
    if (var->array != 0) {
        return (calc_var_size(var->ptr_to) * (var->array->array_size - 1)) ;
    }
    if (var->struct_data != 0) {
        return (calc_var_size(var));
    }
    return 0;
}

void Lvar_offset_calc(List_index* index) {
    List_iter* cur = index->start;
    printf("# DEBUG INFO\n");
    while (cur) {
        Local_var* new_var = cur->data;
        if (cur->prev) {
            Local_var* old_var = cur->prev->data;
            new_var->offset = old_var->offset +  calc_var_size(new_var->type) ;//calc_array_first_offset(new_var->type);
            printf("# %-7.10s: size %-2d :%-3d~ %-3d\n", str_trim(new_var->name, new_var->len),
                                        new_var->size,
                                        new_var->offset - calc_var_size(new_var->type),
                                        new_var->offset);
        } else {
            new_var->offset = 8 + calc_var_size(new_var->type) /*+ calc_array_first_offset(new_var->type)*/;
            printf("# %-7.20s: size %-2d :%-3d~ %-3d\n", str_trim(new_var->name, new_var->len),
                                        new_var->size,
                                        new_var->offset - calc_var_size(new_var->type),
                                        new_var->offset);
        }
        cur = cur->next;
    }
    
}