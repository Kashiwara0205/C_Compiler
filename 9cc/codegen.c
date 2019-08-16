#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("Left side is not variable");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int lbegin_num = 1;
int lend_num = 1;
int lelse_num = 1;
void gen(Node *node){
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_IF:
    gen(node->conditional);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", lend_num);
    gen(node->content);
    printf(".Lend%d:\n", lend_num);
    lend_num+=1;
    return;
  case ND_ELSE:
    gen(node->conditional);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", lelse_num);
    gen(node->content);
    printf("  jmp .Lend%d\n", lend_num);
    printf(".Lelse%d:\n", lelse_num);
    gen(node->else_content);
    printf(".Lend%d:\n", lend_num);
    lelse_num+=1;
    lend_num+=1;
    return;
  case ND_WHILE:
    printf(".Lbegin%d:\n", lbegin_num);
    gen(node->conditional);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", lend_num);
    gen(node->content);
    printf("  jmp .Lbegin%d\n", lbegin_num);
    printf(".Lend%d:\n", lend_num);
    lbegin_num+=1;
    lend_num+=1;
    return;
  case ND_RETURN:
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind){
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NOT_EQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT_EQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_RT:
    printf("  cmp rdi, rax\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_RT_EQ:
    printf("  cmp rdi, rax\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}