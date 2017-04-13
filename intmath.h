#pragma once

typedef enum {
  T_ADD,
  T_SUB,
  T_MUL,
  T_DIV,
  T_NEG,
  T_OPERAND
} PT_NODE_TYPE;

typedef struct PT_NODE PT_NODE;
struct PT_NODE {
  struct PT_NODE *left, *right;
  PT_NODE_TYPE type;
  int operand;
  int reg_num;
};

typedef int (*compiled_expr_fn)(void);

#define NEW_NODE(type,  node)                   \
  do {                                          \
    node = (PT_NODE *) malloc(sizeof(PT_NODE)); \
    node->left = NULL;                          \
    node->right = NULL;                         \
    node->type = type;                          \
    node->reg_num = -1;                         \
 } while(0)

PT_NODE *new_node(PT_NODE_TYPE type);
void assign_registers(PT_NODE *node, int reg_num, int *max_reg);
void print_tree(PT_NODE *node, int lev);
void free_tree(PT_NODE *node);
void print_code(PT_NODE *node);
void assign_registers(PT_NODE *node, int reg_num, int *max_reg);
void tree_walk(PT_NODE *node);
compiled_expr_fn compile_tree(PT_NODE *node, char *argv0);
