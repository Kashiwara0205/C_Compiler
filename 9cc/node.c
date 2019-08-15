#include "9cc.h"

// if next token is symbol, read next token and return true
// otherwise return false
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len || 
      memcmp(token->str, op, token->len)){
    return false;
  }
  token = token->next;

  return true;
}

// if next token is variable, read next token and return token
// otherwise return false
Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return false;
  Token *ident_token;
  // copy ident_token address 
  ident_token = token;
  token = token->next;

  return ident_token;
}

// if next token is return, read next token and return true
// otherwise return false
bool consume_return() {
  if (token->kind != TK_RETURN ||
      strlen("return") != token->len || 
      memcmp(token->str, "return", 6)){
    return false;
  }
  token = token->next;

  return true;
}

// if next token is symbol, read next token
// otherwise notice error
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error("Not '%c'", op);
  token = token->next;
}

// if next token is number, read next token and return value
// totherwise notice error
int expect_number(){
  if (token->kind != TK_NUM)
    error_at(token->str, "Not Number");
  int val = token->val;
  token = token->next;
  return val;
}

// wheter end of file
bool at_eof(){
  return token->kind == TK_EOF;
}

LVar *find_lvar(Token *tok){
  for (LVar *var = locals; var; var = var->next)
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
    return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node -> kind = ND_NUM;
  node -> val = val;
  return node;
}

Node *stmt() {
  Node *node;

  if(consume_return()){
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;     
    node->lhs = expr();
  }else{
   node = expr();
  }

  expect(';');
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NOT_EQ, node, relational());
    else
      return node;
  }

  return node;
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LT_EQ, node, add());
    else if (consume(">"))
      node = new_node(ND_RT, node, add());
    else if (consume(">="))
      node = new_node(ND_RT_EQ, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if (consume("+"))
    return term();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), term());
  return term();
}

Node *term() {
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      // save local variable
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals == NULL){
        lvar->offset = 8;
      }else{
        lvar->offset = locals->offset + 8;
      }
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}