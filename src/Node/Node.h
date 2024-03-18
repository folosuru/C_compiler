#ifndef NODE_NODE_H
#define NODE_NODE_H
#include "../compiler.h"
#include "../Var_type/var_type.h"
typedef enum {
    NODE_ADD,
    NODE_SUB,
    NODE_MLU,
    NODE_DIV,
    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_BIGGER,
    NODE_SMALLER,
    NODE_BIGGER_OR_EQUAL,
    NODE_SMALLER_OR_EQUAL,
    NODE_ASSIGN,
    NODE_LOCALVALUE,
    NODE_ARG,
    NODE_NUM,
    NODE_RETURN,
    NODE_IF,
    NODE_IF_STM,
    NODE_FOR,
    NODE_BLOCK,
    NODE_FUNC_CALL,
    NODE_FUNC_DEF,
    NODE_REFER,
    NODE_DEREFER,
    NODE_DO_NOTHING,
    NODE_TYPE_CONVENTION
} NodeType;
/*
     NODE_IF
        |
   |-----------|
  stm      NODE_IF_STM
                |
           |----------|
        when true    when false
*/


typedef struct Node Node;
struct Node {
    NodeType type;
    Node* left;
    Node* right;
    void* data;
    int value;
    Typename* var_type;
};

typedef struct IF_Node_data IF_Node_data;
struct IF_Node_data{
    Node* condition;
    Node* statement;
    Node* else_statement;
};

typedef struct FOR_Node_data FOR_Node_data;
struct FOR_Node_data {
    Node* init;
    Node* condition;
    Node* iterate;
    Node* code;
};
typedef struct FUNC_CALL_NODE_Data FUNC_CALL_NODE_Data;
struct FUNC_CALL_NODE_Data {
    char* name;
    int len;
    int args_cnt;
    void* args;
};


typedef enum {
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_FOR,
    KEYWORD_WHILE,
    KEYWORD_INT,
    KEYWORD_SIZEOF,
    KEYWORD_CHAR
} Preserved_Word;

typedef struct program Program;
struct program {
    Program* next;
    Node* node;
};

Node* get_NodeTree();
Token* tokenize(char* p);
Program* getProgram();
bool consume_operator(char* operator);
bool consume_preserved(Preserved_Word word);
Token* consume_identify();
char* consume_variable();
Typename* consume_typename(Typename* parent);
int except_number();
void calc(Node* node);
Node* new_node_num(int val);
Node* get_NodeTree();
Node* statement();
Node* expr();
Node* assign_or_declaration();
Node* assign();
Node* equal_operator();
Node* rerational_operator();
Node* add_sub_operator();
Node* mlu();
Node* menber_access() ;
Node* primary();
Node* unary();

Node* type_convention(Node* node, Typename* cast_to);

typedef enum {
    local_value,
    arg_value
} local_value_type;

typedef struct Local_variable Local_var;
struct Local_variable {
    char* name;
    int len;
    int offset;
    Typename* type;
    int size;
};

typedef struct {
    char* name;
    int len;
    int offset;
    int size;
    Typename* type;
    int index;
} Args_var;


typedef struct function_def function_def;
struct function_def {
    Program* program;
    List_iter* args_list;
    int args_count;
    char* name;
    int name_length;

    Typename* return_value;

    //----local var
    List_iter* lvar;
    int total_offset;

    // args
    #ifndef ABI_LINUX
        int args_total_offset;
    #endif
};

int calc_var_size(Typename*);
function_def* getFunction();

#endif