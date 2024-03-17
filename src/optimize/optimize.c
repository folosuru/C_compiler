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
    }
    return node;
}