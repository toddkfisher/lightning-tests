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
};

#define NEW_NODE(type,  node)                   \
  do {                                          \
    node = (PT_NODE *) malloc(sizeof(PT_NODE)); \
    node->left = NULL;                          \
    node->right = NULL;                         \
    node->type = type;                          \
 } while(0)

PT_NODE *new_node(PT_NODE_TYPE type);
void print_tree(PT_NODE *node, int lev);
