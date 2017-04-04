#include <stdlib.h>
#include <stdio.h>
#include "intmath.h"

PT_NODE *new_node(PT_NODE_TYPE type)
{
  PT_NODE *node = (PT_NODE *) malloc(sizeof(PT_NODE));
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

char *operators[] = {
  "+",
  "-",
  "*",
  "/",
  "-",
  "??"
};

void assign_registers(PT_NODE *node, int reg_num, int *max_reg)
{
  if (reg_num > *max_reg) {
    *max_reg = reg_num;
  }
  node->reg_num = reg_num;
  if (T_OPERAND != node->type) {
    if (T_OPERAND != node->right->type) {
      assign_registers(node->left, reg_num, max_reg);
      assign_registers(node->right, reg_num + 1, max_reg);
    } else {
      assign_registers(node->left, reg_num, max_reg);
      // right is constant and does not get a register number
      // instruction generated will be Rn = Rn + immediate const
      //                                       (from right branch)
      assign_registers(node->right, -1, max_reg);
    }
  }
}

void free_tree(PT_NODE *node)
{
  if (T_OPERAND != node->type) {
    free_tree(node->left);
    free_tree(node->right);
  }
  free(node);
}

void print_code(PT_NODE *node)
{
  int reg_num = node->reg_num;
  if (T_OPERAND == node->type) {
    printf("R%d <- %d\n", reg_num, node->operand);
  } else {
    char *op = operators[node->type];
    if (-1 == node->right->reg_num) {
      print_code(node->left);
      printf("R%d <- R%d %s %d\n", reg_num, reg_num, op, node->right->operand);
    } else {
      print_code(node->right);
      print_code(node->left);
      printf("R%d <- R%d %s R%d\n", reg_num, reg_num, op, node->right->reg_num);
    }
  }
}

void print_tree(PT_NODE *node, int lev)
{
  int indent = 2*lev;
  while (indent--) {
    printf(" ");
  }
  printf("[%02d] ", lev);
  if (-1 == node->reg_num) {
    printf("Imm : ");
  } else {
    printf("R%02d : ", node->reg_num);
  }
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
