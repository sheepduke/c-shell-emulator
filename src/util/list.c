#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// ----------------------------------------------------------------------
// node definition
// ----------------------------------------------------------------------

typedef struct Node Node;

struct Node {
  void *data;
  Node *prev;
  Node *next;
};

static Node *node_new(void *data) {
  Node *node = malloc(sizeof(node));
  assert(node);

  node->data = data;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

static void node_destroy(Node *node) {
  free(node->data);
  free(node);
}

struct List {
  size_t size;
  Node *head;
  Node *tail;
};

// ----------------------------------------------------------------------
// internal function
// ----------------------------------------------------------------------

void link_node(Node *left, Node *right) {
  if (left) {
	left->next = right;
  }
  if (right) {
	right->prev = left;
  }
}

// ----------------------------------------------------------------------
// API
// ----------------------------------------------------------------------

List *list_new() {
  List *list = malloc(sizeof(List));
  assert(list);
  
  list->size = 0;
  list->head = list->tail = NULL;
  return list;
}

void list_destroy(List *list) {
  Node *node = list->head;
  Node *temp = node;

  while (node) {
    temp = node->next;
    node_destroy(node);
    node = temp;
  }
  free(list);
}

size_t list_size(const List *list) {
  assert(list);
  return list->size;
}

void *list_front(const List *list) {
  assert(list);
  return (list->head ? list->head->data : NULL);
}

void *list_back(const List *list) {
  assert(list);
  return (list->tail ? list->tail->data : NULL);
}

void list_foreach(const List *list, void (*func)(void *data)) {
  assert(list);
  for (Node *node = list->head; node; node = node->next) {
	func(node->data);
  }
}

List *list_push_back(List *list, void *data) {
  assert(list && data);
  Node *new_node = node_new(data);
  if (list->tail) {
	link_node(list->tail, new_node);
	list->tail = list->tail->next;
  }
  else {
	list->tail = list->head = new_node;
  }
  list->size++;
  return list;
}

List *list_push_front(List *list, void *data) {
  assert(list && data);
  
  Node *new_node = node_new(data);
  
  if (list->head) {
	link_node(new_node, list->head);
	list->head = new_node;
  }
  else {
	list->head = list->tail = new_node;
  }
  list->size++;
  return list;
}

void list_pop_front(List *list) {
  assert(list);
  if (list->head) {
	Node *old_head = list->head;
	list->head = list->head->next;
	node_destroy(old_head);
  }
  list->size--;
}

void list_pop_back(List *list) {
  assert(list);
  if (list->tail) {
	Node *old_tail = list->tail;
	list->tail = list->tail->prev;
	node_destroy(old_tail);
  }
  list->size--;
}

void list_clear(List *list) {
  assert(list);
  list_foreach(list, free);
  list->head = list->tail = NULL;
  list->size = 0;
}
