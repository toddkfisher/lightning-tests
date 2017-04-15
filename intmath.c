#include <stdlib.h>
#include <stdio.h>
#include <lightning.h>
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

int regs[] = {
  JIT_R0,
  JIT_R1,
  JIT_R2,
  JIT_V0,
  JIT_V1,
  JIT_V2
};

static jit_state_t *_jit;

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

#define GEN_OP(i_or_r, node, dest, right)                    \
do {                                                         \
  switch(node->type) {                                       \
    case T_ADD:                                              \
      jit_add##i_or_r(dest, dest, right);                    \
      break;                                                 \
    case T_SUB:                                              \
      jit_sub##i_or_r(dest, dest, right);                    \
      break;                                                 \
    case T_MUL:                                              \
      jit_mul##i_or_r(dest, dest, right);                    \
      break;                                                 \
    case T_DIV:                                              \
      jit_div##i_or_r(dest, dest, right);                    \
      break;                                                 \
    default:                                                 \
      fprintf(stderr, "Unknown operator: %d\n", node->type); \
      exit(0);                                               \
      break;                                                 \
  }                                                          \
} while (0)

void tree_walk(PT_NODE *node)
{
  int jit_dest_reg = regs[node->reg_num],
    pr_dest_reg = node->reg_num;
  if (T_OPERAND == node->type) {
    printf("R%d <- %d\n", pr_dest_reg, node->operand);
    jit_movi(jit_dest_reg, node->operand);
  } else {
    char *op = operators[node->type];
    if (T_OPERAND == node->right->type) {
      tree_walk(node->left);
      printf("R%d <- R%d %s %d\n", pr_dest_reg, pr_dest_reg, op,
             node->right->operand);
      GEN_OP(i, node, jit_dest_reg, node->right->operand);
    } else {
      tree_walk(node->right);
      tree_walk(node->left);
      printf("R%d <- R%d %s R%d\n", pr_dest_reg, pr_dest_reg, op,
             node->right->reg_num);
      GEN_OP(r, node, jit_dest_reg, regs[node->right->reg_num]);
    }
  }
}

compiled_expr_fn compile_tree(PT_NODE *node, char *argv0)
{
  compiled_expr_fn fn = NULL;
  init_jit(argv0);
  _jit = jit_new_state();
  jit_prolog();
  tree_walk(node);
  jit_retr(regs[0]);
  jit_epilog();
  fn = jit_emit();
  jit_disassemble();
  return fn;
}

void print_tree(PT_NODE *node, int lev)
{
  int indent = 2*lev;
  while (indent--) {
    printf(" ");
  }
  printf("[%1d] ", lev);
  if (-1 == node->reg_num) {
    printf("Imm : ");
  } else {
    printf("R%1d : ", node->reg_num);
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
