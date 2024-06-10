#include "./optimize.h"
Node* optimize(Node* node) {
    if (node->type == NODE_MLU) {
        Node* left = optimize(node->left);
        Node* right = optimize(node->right);
        if (left->type == NODE_NUM && right->type == NODE_NUM) {
            return new_node_num(left->value * right->value);
        }
        if ((left->type == NODE_NUM && left->value == 0) || (right->type == NODE_NUM && right->value == 0)) {
            return new_node_num(0);
        }
    }
    if (node->type == NODE_ADD) {
        Node* left = optimize(node->left);
        Node* right = optimize(node->right);
        if (left->type == NODE_NUM && right->type == NODE_NUM) {
            return new_node_num(left->value + right->value);
        }
        if (left->type == NODE_NUM && left->value == 0) {
            return right;
        }
        if (right->type == NODE_NUM && right->value == 0) {
            return left;
        }
    }
    return node;
}


bool optimize_push(List_iter* push_iter) {
    Asm_statement* current_var = push_iter->data;
    if (push_iter->next != 0) {
        Asm_statement* next_var = push_iter->next->data;

        if (next_var->instruction.type != value_preserved) {
            return false;
        }
        if (next_var->instruction.value.preserved == pop) {
            if (current_var->operand1 == next_var->operand1) {
                if (push_iter->prev != 0) {
                    push_iter->prev->next = push_iter->next->next;
                }
                if (push_iter->next->next != 0) {
                    push_iter->next->next->prev = push_iter->prev;
                }
                push_iter->next->data = 0;
                return true;
            }
            
            current_var->instruction.value.preserved = mov;
            operand* dest = next_var->operand1;
            operand* from = current_var->operand1;
            current_var->operand1 = dest;
            current_var->operand2 = from;
            
            if (push_iter->next->next != 0) {
                push_iter->next->next->prev = push_iter;
            }
            push_iter->next = push_iter->next->next;
            return true;
        }
    }
    return false;
}


bool optimize_chained_asm(List_iter* current) {
    Asm_statement* current_var = current->data;
    if (current->next != 0) {
        List_iter* next = current->next;
        Asm_statement* next_var = next->data;
        if (current_var->instruction.type != value_preserved || next_var->instruction.type != value_preserved) {
            return false;
        }
        if (current_var->operand1 == 0 || current_var->operand2 == 0) {
            return false;
        }
        if (current_var->operand1->type != value_preserved || next_var->operand1->type != value_preserved) {
            return false;
        }
        if (next_var->operand1->value.redister.word != current_var->operand1->value.redister.word) {
            return false;
        }
        if (current_var->instruction.value.preserved == next_var->instruction.value.preserved) {
            if (current_var->instruction.value.preserved == add || next_var->instruction.value.preserved == sub) {
                if (current_var->operand2->type == value_num && next_var->operand2->type == value_num) {
                    current_var->operand2->value.number += next_var->operand2->value.number;
                    //next_var->operand2->value.number = 0;
                    
                    if (next->next != 0) {
                        next->next->prev = current;
                        current->next = next->next;
                    }
                    //current->next->data = 0;
                    return true;
                }
            }
        } 
    }
    return false;
}

void optimize_asm(List_index* asm_source) {
    if (asm_source == 0) {
        return;
    }
    List_iter* current = asm_source->start;
    int change_count = 0;
    for ( ; current != 0; current = current->next ) {
        if (current->data == 0) { continue; }
        Asm_statement* current_var = current->data;
        if (current_var->instruction.type != value_preserved) {
            continue;
        }
        if (current_var->instruction.value.preserved == push) {
            if (optimize_push(current)) {
                change_count++;
                continue;
            }
        }
        if (optimize_chained_asm(current)) {
            change_count++;
        }
    }
    if (change_count != 0) {
        optimize_asm(asm_source);
    }
    return;
}