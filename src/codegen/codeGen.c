#include "../compiler.h"
#include "../Node/Node.h"
#include "printAsm.h"
#include "AsmData.h"
#include "../util/array_util.h"
#include "../util/string_util.h"
#include "../optimize/optimize.h"

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
        create_asm_statement_enum(mov, create_operand_redister(rax, 8), create_operand_redister(rbp, 8));
        // printf("  sub rax, %d  # variable %s\n", get_node_offset(node), str_trim(((Local_var*)node->data)->name, ((Local_var*)node->data)->len));
        create_asm_statement_enum(sub, create_operand_redister(rax, 8), create_operand_num(get_node_offset(node)));
    } else if ((node->type == NODE_ARG)) {
        //printf("  mov rax, rbp\n");
        create_asm_statement_enum(mov, create_operand_redister(rax, 8), create_operand_redister(rbp, 8));
        //printf("  add rax, %d  # variable %s\n", get_node_offset(node), str_trim(((Args_var*)node->data)->name, ((Args_var*)node->data)->len));
        create_asm_statement_enum(add, create_operand_redister(rax, 8), create_operand_num(get_node_offset(node)));
    } else if (node->type == NODE_GLOBAL_VAR ) {
        Globalvar_def* global_var = node->data;
        //printf("  lea rax, [rip + %s]  # global variable %s\n", global_var->name, global_var->name);
        create_asm_statement_text("lea", create_operand_redister(rax, 8), create_operand_text_fmt("[rip + %s]", global_var->name));
    } else if (node->type == NODE_REFER) {
        calc(node->left);
        print_pop(rax);
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
    return get_action_size_prefix(calc_var_redister_size(node->var_type));
}

void calc(Node* node_) {
    Node* node = optimize(node_);
    switch (node->type) {
        case NODE_DO_NOTHING : {
            return ;
        }

        case NODE_NUM: {
            print_push_number(node->value);
            return;
        }
        case NODE_STRING_LITERAL: {
            create_asm_statement_text("lea", 
                                      create_operand_redister(rax, 8),
                                      create_operand_text_fmt("[ rip + .Ltext%d ]", node->value) );
            print_push_register(rax);
            return;
        }
        case NODE_LOCALVALUE: {
            if (calc_var_redister_size(node->var_type) >= 4) {
                //printf("  mov %s, %s PTR  [rbp - %d] # lvar: rvar %s \n", getRedisterName(rax, calc_var_redister_size(node->var_type)), get_size_word_node(node),get_node_offset(node), str_trim(((Local_var*)node->data)->name, ((Local_var*)node->data)->len));
                create_asm_statement_enum(mov, 
                                          create_operand_redister(rax, calc_var_redister_size(node->var_type)),
                                          create_operand_text_fmt("%s PTR  [rbp - %d]", get_size_word_node(node),get_node_offset(node)));
            }
            if (calc_var_redister_size(node->var_type) < 4) {
                //printf("  movzx %s, %s PTR  [rbp - %d] # lvar: rvar %s \n", getRedisterName(rax, 8/*calc_var_redister_size(node->var_type)*/), get_size_word_node(node),get_node_offset(node), str_trim(((Local_var*)node->data)->name, ((Local_var*)node->data)->len));
                create_asm_statement_enum(movzx, 
                                          create_operand_redister(rax, 8),
                                          create_operand_text_fmt("%s PTR  [rbp - %d]", get_size_word_node(node),get_node_offset(node)));
            }
            print_push_register(rax);
            return;
        }
        case NODE_ARG: {
            if (calc_var_redister_size(node->var_type) >= 4) {
                //printf("  mov %s, %s PTR [rbp + %d]\n", getRedisterName(rax, calc_var_redister_size(node->var_type)), get_size_word_node(node), get_node_offset(node));
                create_asm_statement_enum(mov, 
                          create_operand_redister(rax, calc_var_redister_size(node->var_type)),
                          create_operand_text_fmt("%s PTR  [rbp + %d]", get_size_word_node(node),get_node_offset(node)));
            } else {
                create_asm_statement_enum(movzx, 
                          create_operand_redister(rax, calc_var_redister_size(node->var_type)),
                          create_operand_text_fmt("%s PTR  [rbp + %d]", get_size_word_node(node),get_node_offset(node)));
            }
            print_push_register(rax);
            return;
        }
        case NODE_GLOBAL_VAR: {
            Globalvar_def* global_var = node->data;
            /*printf("  mov %s, %s PTR [rip + %s]\n", 
                    getRedisterName(rax, calc_var_redister_size(node->var_type)),
                    get_size_word_node(node),
                    global_var->name);*/
            create_asm_statement_enum(mov, 
                                      create_operand_redister(rax, calc_var_redister_size(node->var_type)),
                                      create_operand_text_fmt("%s PTR [rip + %s]", get_size_word_node(node), global_var->name));
            print_push_register(rax);
            return;
        }
        case NODE_ASSIGN :{
            if (node->left->type == NODE_LOCALVALUE) {
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
                //printf("  mov %s PTR [rbp - %d], %s # assign %s \n", get_size_word_node(node->left), get_node_offset(node->left), getRedisterName(rdi, calc_var_redister_size(node->left->var_type)), str_trim(((Local_var*)node->left->data)->name, ((Local_var*)node->left->data)->len));
                create_asm_statement_enum(mov,
                                          create_operand_text_fmt("%s PTR [rbp - %d]", get_size_word_node(node->left), get_node_offset(node->left)),
                                          create_operand_redister(rdi, calc_var_redister_size(node->left->var_type)));

            } else if (node->left->type == NODE_ARG) {
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
//                printf("  mov %s PTR [rbp + %d], %s\n", get_size_word_node(node->left), get_node_offset(node->left), getRedisterName(rdi, calc_var_redister_size(node->left->var_type)));

                create_asm_statement_enum(mov,
                    create_operand_text_fmt("%s PTR [rbp + %d]", get_size_word_node(node->left), get_node_offset(node->left)),
                    create_operand_redister(rdi, calc_var_redister_size(node->left->var_type)));

            } else if (node->left->type == NODE_GLOBAL_VAR) {
                Globalvar_def* global_var = node->left->data;
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
//                printf("  mov %s PTR [rip + %s], %s\n", get_size_word_node(node->left), global_var->name , getRedisterName(rdi, calc_var_redister_size(node->left->var_type)));

                create_asm_statement_enum(mov, 
                    create_operand_text_fmt("%s PTR [rip + %s]", get_size_word_node(node->left), global_var->name),
                    create_operand_redister(rdi, calc_var_redister_size(node->left->var_type)));

            } else if (node->left->type == NODE_REFER) {
                push_lval_ptr(node->left);  // rax
                calc(node->right);  // こっちがrdi
                print_pop(rdi);
                print_pop(rax);
                //printf("  mov %s PTR [rax], %s\n", get_size_word_node(node->left), getRedisterName(rdi, calc_var_redister_size(node->left->var_type)));
                create_asm_statement_enum(mov, 
                    create_operand_text_fmt("%s PTR [rax]", get_size_word_node(node->left)),
                    create_operand_redister(rdi, calc_var_redister_size(node->left->var_type)));
            }
            print_push_register(rdi);
            return;;
        }
        case NODE_RETURN : {
            calc(node->left);
            print_pop(rax);
            //printf("  mov rsp, rbp\n");
            create_asm_statement_enum(mov, create_operand_redister(rsp, 8), create_operand_redister(rbp, 8));
            print_pop(rbp);
            //printf("  ret\n");
            create_asm_statement_text("ret", 0,0);
            return;
        }
        case NODE_IF : {
            int label_cnt = gen_Label_no();
            IF_Node_data* data = node->data;
            calc(data->condition);
            print_pop(rax);
            //printf("  cmp rax, 0\n");  

            // is false
            create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_num(0));
            if (data->else_statement == NULL) {
                //printf("  je  .Lend%d\n", label_cnt);
                create_asm_statement_enum(je, create_operand_text_fmt(".Lend%d", label_cnt), 0);
                calc(data->statement);
            } else {
                //printf("  je  .Lelse%d\n", label_cnt);
                create_asm_statement_enum(je, create_operand_text_fmt(".Lelse%d", label_cnt), 0);
                calc(data->statement);
                //printf("  jmp .Lend%d\n", label_cnt);
                create_asm_statement_enum(jmp, create_operand_text_fmt(".Lend%d", label_cnt), 0);
                //printf(".Lelse%d:\n", label_cnt);
                create_asm_statement_directive_fmt(".Lelse%d:", label_cnt);
                
                calc(data->else_statement);
            }
            //printf(".Lend%d:\n", label_cnt);
            create_asm_statement_directive_fmt(".Lend%d:", label_cnt);
            return;
        }
        case NODE_FOR: {
            int label_cnt = gen_Label_no();
            FOR_Node_data* data = node->data;
            if (data->init != 0) {
                calc(data->init);
            }
            //printf(".Lstart%d:\n", label_cnt);
            create_asm_statement_directive_fmt(".Lstart%d:", label_cnt);
            if (data->condition != 0) {
                calc(data->condition);
            }
            print_pop(rax);
            //printf("  cmp rax, 0\n");  // is false
            create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_num(0));
            //printf("  je  .Lend%d\n", label_cnt);
            create_asm_statement_enum(je, create_operand_text_fmt(".Lend%d", label_cnt), 0);
            calc(data->code);
            if (data->iterate != 0) {
                calc(data->iterate);
            }
            //printf("  jmp .Lstart%d\n", label_cnt);
            create_asm_statement_enum(jmp, create_operand_text_fmt(".Lstart%d", label_cnt), 0);

            //printf(".Lend%d:\n", label_cnt);
            create_asm_statement_directive_fmt(".Lend%d:", label_cnt);
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
            //printf(" sub rsp, 32\n");
            create_asm_statement_enum(sub, create_operand_redister(rsp, 8), create_operand_num(32));
            //printf("  call %s\n", function_name);
            create_asm_statement_text("call", create_operand_text_fmt("%s\n", function_name), 0);
            //printf(" add rsp, 32\n");
            create_asm_statement_enum(add, create_operand_redister(rsp, 8), create_operand_num(32));
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
            print_pop(rax);
            if (calc_var_redister_size(node->var_type) >= 4) {
                //printf("  mov %s, %s PTR [rax]\n", getRedisterName(rax, calc_var_redister_size(node->var_type)),
                //                                   get_action_size_prefix(calc_var_redister_size(node->var_type)));
                create_asm_statement_enum(mov, 
                    create_operand_redister(rax, calc_var_redister_size(node->var_type)),
                    create_operand_text_fmt("%s PTR [rax]", get_action_size_prefix(calc_var_redister_size(node->var_type))));
            } else {
                //printf("  movsx %s, %s PTR [rax]\n", getRedisterName(rax, 8),
                //                                   get_action_size_prefix(calc_var_redister_size(node->var_type)));
                create_asm_statement_enum(movsx, 
                    create_operand_redister(rax, 8),
                    create_operand_text_fmt("%s PTR [rax]", get_action_size_prefix(calc_var_redister_size(node->var_type))));

            }
            print_push_register(rax);
            return;
        }
        case NODE_TYPE_CONVENTION: {
            calc(node->left);
            if (calc_var_redister_size(node->left->var_type) == 4)  {
                return;
            }
            print_pop(rax);
            //printf("  movsx rax, %s\n", getRedisterName(rax, calc_var_redister_size(node->left->var_type)));
            create_asm_statement_enum(movsx, 
                create_operand_redister(rax, 8),
                create_operand_redister(rax, calc_var_redister_size(node->left->var_type)));
            print_push_register(rax);
            return;
        }

        default: {
            break;
        }
    }


    calc(node->left);
    calc(node->right);
    print_pop(rdi);
    print_pop(rax);

    switch (node->type){
    case NODE_ADD: {
        //printf("  add rax, rdi\n");
        create_asm_statement_enum(add,
            create_operand_redister(rax, 8),
            create_operand_redister(rdi,8));
        break;
    }
    case NODE_SUB: {
        //printf("  sub rax, rdi\n");
        create_asm_statement_enum(sub, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        break;
    }
    case NODE_MLU: {
        //printf("  imul rax, rdi\n");
        create_asm_statement_enum(imul, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        break;
    }
    case NODE_DIV: {
        //printf("  cqo\n");
        create_asm_statement_enum(cqo, 0, 0);
        //printf("  idiv rax, rdi\n");
        create_asm_statement_enum(idiv, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        break;
    }
    case NODE_EQUAL: {
        //printf("  cmp rax, rdi\n");
        create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        //printf("  sete al\n");
        create_asm_statement_enum(sete, create_operand_redister(rax, 1), 0);
        //printf("  movzb rax, al\n");
        create_asm_statement_enum(movzb, create_operand_redister(rax, 8), create_operand_redister(rax, 1));
        break;
    }
    case NODE_NOT_EQUAL: {
        //printf("  cmp rax, rdi\n");
        create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        //printf("  setne al\n");
        create_asm_statement_enum(setne, create_operand_redister(rax, 1), 0);
        //printf("  movzb rax, al\n");
        create_asm_statement_enum(movzb, create_operand_redister(rax, 8), create_operand_redister(rax, 1));
        break;
    }
    case NODE_SMALLER: {
        //printf("  cmp rax, rdi\n");
        create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        //printf("  setl al\n");
        create_asm_statement_enum(setl, create_operand_redister(rax, 1), 0);
        //printf("  movzb rax, al\n");
        create_asm_statement_enum(movzb, create_operand_redister(rax, 8), create_operand_redister(rax, 1));
        break;
    }
    case NODE_SMALLER_OR_EQUAL: {
        //printf("  cmp rax, rdi\n");
        create_asm_statement_enum(cmp, create_operand_redister(rax, 8), create_operand_redister(rdi, 8));
        //printf("  setle al\n");
        create_asm_statement_enum(setle, create_operand_redister(rax, 1), 0);
        //printf("  movzb rax, al\n");
        create_asm_statement_enum(movzb, create_operand_redister(rax, 8), create_operand_redister(rax, 1));
        break;
    }
    default:
        break;
    }
    print_push_register(rax);
    return;
}