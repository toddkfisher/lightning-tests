#include <stdlib.h>
#include <stdio.h>
#include "intmath.h"

PT_NODE *new_node(PT_NODE_TYPE type)
{
  PT_NODE *node = (PT_NODE *) malloc(sizeof(PT_NODE *));
  node->type = type;
  node->left = node->right = NULL;
  return node;
}

char *operator_names[] = {
  "T_ADD",
  "T_SUB",
  "T_MUL",
  "T_DIV",
  "T_NEG",
  "T_OPERAND"
};

void print_tree(PT_NODE *node, int lev)
{
  int indent = 2*lev;
  while (indent--) { printf(" "); }
  printf("[%02d] ", lev);
  switch (node->type) {
    case T_ADD:
    case T_SUB:
    case T_MUL:
    case T_DIV:
      printf("%s\n", operator_names[node->type]);
      print_tree(node->left, lev + 1);
      print_tree(node->right, lev + 1);
      break;
    case T_NEG:
      printf("negate\n");
      print_tree(node->left, lev + 1);
      break;
    case T_OPERAND:
      printf("T_OPERAND : %d\n", node->operand);
      break;
    default:
      printf("unknown node type: %d\n", node->type);
      break;
  }
}
