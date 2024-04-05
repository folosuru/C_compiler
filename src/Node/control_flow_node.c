#include "Node.h"
Node* if_node(){
        Node* node = calloc(1, sizeof(Node));
        node->data = calloc(1, sizeof(IF_Node_data));
        IF_Node_data* data = node->data;
        node->type = NODE_IF;
        data->condition = assign();
        data->statement = statement();
        if (consume_preserved(KEYWORD_ELSE)) {
            data->else_statement = statement();
        }
        return node;
}

Node* for_node() { 
    Node* node = calloc(1, sizeof(Node));
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

}