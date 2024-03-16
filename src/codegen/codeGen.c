#include "../compiler.h"
#include "../Node/Node.h"
#include "printAsm.h"
#include "../util/array_util.h"
#include "../util/string_util.h"

int get_node_offset(Node* node) {
    switch (node->type){
    case NODE_LOCALVALUE: {
        Local_var* local = node->data;
        return local->offset;
        break;
    }
    case NODE_ARG: {
        Args_var* local = node->data;
        return local->offset;
        break;
    }
    default:
        break;
    }
}

void push_lval_ptr(Node* node) {
    if (node->type == NODE_LOCALVALUE) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d  # variable %s\n", get_node_offset(node), str_trim(((Local_var*)node->data)->name, ((Local_var*)node->data)->len));
    } else if (node->type == NODE_REFER) {
        push_lval_ptr(node->left);
        print_pop(rdi);
        printf("  mov rax, [rdi] # refer\n ");
    } else {
        error("is not variable %d", node->type);
    }
    print_push_register(rax);
}
int label_count;
int gen_Label_no() {
    label_count++;
    return label_count-1;
}

char* get_size_word_node(Node* node) {
    return get_action_size_prefix(calc_var_size(node->var_type));
}

void calc(Node* node) {
    switch (node->type) {
        case NODE_DO_NOTHING : {
            return ;
        }

        case NODE_NUM: {
            print_push_number(node->value);
            return;
        }
        case NODE_LOCALVALUE: {
            printf("  mov %s, %s PTR  [rbp - %d] # lvar: rvar %s \n", getRedisterName(rax, calc_var_size(node->var_type)), get_size_word_node(node),get_node_offset(node), str_trim(((Local_var*)node->data)->name, ((Local_var*)node->data)->len));
            print_push_register(rax);
            return;
        }
        case NODE_ARG: {
            printf("  mov %s, %s PTR [rbp + %d]\n", getRedisterName(rax, calc_var_size(node->var_type)), get_size_word_node(node), get_node_offset(node));
            print_push_register(rax);
            return;
        }
        case NODE_ASSIGN :{
            if (node->left->type == NODE_LOCALVALUE) {
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
                printf("  mov %s PTR [rbp - %d], %s # assign %s \n", get_size_word_node(node->left), get_node_offset(node->left), getRedisterName(rdi, calc_var_size(node->left->var_type)), str_trim(((Local_var*)node->left->data)->name, ((Local_var*)node->left->data)->len));
            } else if (node->left->type == NODE_ARG) {
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
                printf("  mov %s PTR [rbp + %d], %s\n", get_size_word_node(node->left), get_node_offset(node->left), getRedisterName(rdi, calc_var_size(node->left->var_type)));
            } else if (node->left->type == NODE_REFER) {
                push_lval_ptr(node->left);  // rax
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
                print_pop(rax);
                printf("  mov %s PTR [rax], %s\n", get_size_word_node(node->left), getRedisterName(rdi, calc_var_size(node->left->var_type)));
            }
            print_push_register(rdi);
            return;;
        }
        case NODE_RETURN : {
            calc(node->left);
            print_pop(rax);
            printf("  movsx rax, eax\n");
            printf("  mov rsp, rbp\n");
            print_pop(rbp);
            printf("  ret\n");
            return;
        }
        case NODE_IF : {
            int label_cnt = gen_Label_no();
            IF_Node_data* data = node->data;
            calc(data->condition);
            print_pop(rax);
            printf("  cmp rax, 0\n");  // is false
            if (data->else_statement == NULL) {
                printf("  je  .Lend%d\n", label_cnt);
                calc(data->statement);
            } else {
                printf("  je  .Lelse%d\n", label_cnt);
                calc(data->statement);
                printf("  jmp .Lend%d\n", label_cnt);

                printf(".Lelse%d:\n", label_cnt);
                calc(data->else_statement);
            }
            printf(".Lend%d:\n", label_cnt);
            return;
        }
        case NODE_FOR: {
            int label_cnt = gen_Label_no();
            FOR_Node_data* data = node->data;
            if (data->init != 0) {
                calc(data->init);
            }
            printf(".Lstart%d:\n", label_cnt);
            if (data->condition != 0) {
                calc(data->condition);
            }
            print_pop(rax);
            printf("  cmp rax, 0\n");  // is false
            printf("  je  .Lend%d\n", label_cnt);
            calc(data->code);
            if (data->iterate != 0) {
                calc(data->iterate);
            }
            printf("  jmp .Lstart%d\n", label_cnt);
            printf(".Lend%d:\n", label_cnt);
            return;
        }
        case NODE_BLOCK : {
            Program* data = node->data;
            while (data) {
                if (data->node != NULL) {
                    calc(data->node);
                }
                data = data->next;
            }
            return;
        }
        case NODE_FUNC_CALL: {
            FUNC_CALL_NODE_Data* data = node->data;
            char* function_name = str_trim(data->name, data->len);
            List_iter* args_list = data->args;
            int args_count = data->args_cnt;
            align_rsp(data->args_cnt);
            
            while (args_list) {
                if (args_list->data) {
                    calc((Node*)(args_list->data));
                    redister_word place_for = get_args_place_for(args_count);
                    if (place_for != stack) {
                        print_pop(place_for);
                    }
                    args_count--;
                } 
                if (args_list->prev != 0) {
                    args_list = args_list->prev;
                } else {
                    break;
                }
            }
            printf(" sub rsp, 32\n");
            printf("  call %s\n", function_name);
            un_align_rsp(data->args_cnt);
            print_push_register(rax);
            return;
        }
        case NODE_DEREFER : {
            push_lval_ptr(node->left);
            return;
        }
        case NODE_REFER : {
            calc(node->left);
            printf("  pop rax #ref\n");
            printf("  mov %s, %s PTR [rax]\n", getRedisterName(rax, calc_var_size(refer_ptr(node->left->var_type))), get_action_size_prefix(calc_var_size(refer_ptr(node->left->var_type))));
            printf("  push rax\n");
            return;
        }
        case NODE_TYPE_CONVENTION: {
            calc(node->left);
            print_pop(rax);
            printf("  movsx rax, %s\n", getRedisterName(rax, calc_var_size(node->left->var_type)));
            print_push_register(rax);
            return;
        }

        default: {
            break;
        }
    }


    calc(node->left);
    calc(node->right);
    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->type){
    case NODE_ADD: {
        printf("  add rax, rdi\n");
        break;
    }
    case NODE_SUB: {
        printf("  sub rax, rdi\n");
        break;
    }
    case NODE_MLU: {
        printf("  imul rax, rdi\n");
        break;
    }
    case NODE_DIV: {
        printf("  cqo\n");
        printf("  idiv rax, rdi\n");
        break;
    }
    case NODE_EQUAL: {
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    }
    case NODE_NOT_EQUAL: {
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    }
    case NODE_SMALLER: {
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    }
    case NODE_SMALLER_OR_EQUAL: {
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }
    default:
        break;
    }
    print_push_register(rax);
    return;
}